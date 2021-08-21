mod byond;
mod functions;
mod gdt;
mod sigscan;
mod stack_op;
mod value;

use std::cell::RefCell;
use std::collections::HashSet;
use std::convert::TryFrom;
use std::ffi::OsStr;
use std::mem::size_of;
use std::ops::Deref;
use std::path::Path;
use std::rc::Rc;

use unicorn::unicorn_const::{Arch, HookType, Mode, Permission, SECOND_SCALE};
use unicorn::{RegisterX86, Unicorn, UnicornHandle, X86Mmr};

use minidump::{Minidump, MinidumpMemory, MinidumpMemoryList, MinidumpModuleList};

use crate::stack_op::StackOp;
use crate::value::Value;

// TODO: dynamic allocations for all of these
const STACK_ADDRESS: u64 = 0x80000000;
const STACK_SIZE: usize = 0x1000;
const GDT_ADDRESS: u64 = STACK_ADDRESS;
const TIB_ADDRESS: u64 = 0x1000;
const TIB_SIZE: usize = 0x1000;

struct ByondEmulator {
    unicorn: Unicorn,

    // Our state needs to be shared with hooks that may be run while unicorn is borrowed
    state: Rc<RefCell<ByondState>>,
}

struct ByondState {
    dump: minidump::Minidump<'static, minidump::Mmap>,

    // base addresses of memory regions that have already been mapped in
    mapped: HashSet<u64>,

    // addresses of byond functions
    functions: functions::Functions,
}

impl ByondEmulator {
    fn new(path: &Path) -> Self {
        let unicorn = unicorn::Unicorn::new(Arch::X86, Mode::MODE_32)
            .expect("failed to initialize Unicorn instance");

        let dump = minidump::Minidump::read_path(path).expect("failed to load dump");

        let mut this = Self {
            unicorn,
            state: Rc::new(RefCell::new(ByondState {
                dump,
                mapped: HashSet::new(),
                functions: Default::default(),
            })),
        };

        {
            // Stack
            {
                let mut emu = this.unicorn.borrow();
                emu.mem_map(STACK_ADDRESS, STACK_SIZE, Permission::ALL)
                    .expect("failed to allocate stack");
                this.stack_clear();
            }

            

            // GDT (poor man's version)
            {
                let mut emu = this.unicorn.borrow();
                let gdt = gdt::create_basic_gdt();
                emu.mem_write(GDT_ADDRESS, &gdt).unwrap();

                let gdtr: X86Mmr = X86Mmr {
                    selector: 0,
                    base: GDT_ADDRESS,
                    limit: 5 * 8,
                    flags: 0,
                };
                unsafe {
                    let gdtr = std::slice::from_raw_parts::<u8>(
                        &gdtr as *const _ as *const u8,
                        std::mem::size_of::<X86Mmr>(),
                    );
                    emu.reg_write_long(RegisterX86::GDTR as i32, gdtr.to_vec().into_boxed_slice())
                        .unwrap();
                }

                let cs_ds_es = gdt::create_selector(3, 0x3);
                emu.reg_write(RegisterX86::CS as i32, cs_ds_es as u64)
                    .unwrap();
                emu.reg_write(RegisterX86::DS as i32, cs_ds_es as u64)
                    .unwrap();
                emu.reg_write(RegisterX86::ES as i32, cs_ds_es as u64)
                    .unwrap();

                let ss = gdt::create_selector(2, 0x0);
                emu.reg_write(RegisterX86::SS as i32, ss as u64).unwrap();

                let fs = gdt::create_selector(4, 0x3);
                emu.reg_write(RegisterX86::FS as i32, fs as u64).unwrap();
            }

            // TIB (extreme poor man's version)
            {
                let mut emu = this.unicorn.borrow();
                emu.mem_map(TIB_ADDRESS, TIB_SIZE, Permission::ALL)
                    .expect("failed to allocate tib");
                emu.mem_write(TIB_ADDRESS + 0x18, &(TIB_ADDRESS as u32).to_le_bytes())
                    .unwrap();
            }


            {
                let mut emu = this.unicorn.borrow();

                let state = Rc::clone(&this.state);
                emu.add_mem_invalid_hook(HookType::MEM_INVALID, 0, 0xFFFFFFFFFFFFFFFF, move |mut emu, a, b, c, d| {
                    Self::ensure_mapped(&mut emu, &mut state.borrow_mut(), b, c)
                })
                .unwrap();
            }


            // Kind of... bad
            this.state.borrow_mut().functions = functions::Functions::new(&mut this);
        }

        this
    }

    fn stack_clear(&mut self) {
        let mut emu = self.unicorn.borrow();

        // (≻‿≺ 乂 da
        // 乂 ≻‿≺) da
        // (≻‿≺ 乂 da
        // 乂 ≻‿≺) da
        emu.mem_write(STACK_ADDRESS, &[0xda; STACK_SIZE]).unwrap();

        emu.reg_write(RegisterX86::ESP as i32, STACK_ADDRESS + STACK_SIZE as u64)
            .unwrap();
    }

    fn stack_push<T: StackOp>(&mut self, value: T) {
        value.push(&mut self.unicorn.borrow());
    }

    fn stack_pop<T: StackOp>(&mut self) -> T {
        T::pop(&mut self.unicorn.borrow())
    }

    // searches for a module in the dump and returns its memory region
    fn find_module(&self, name: &str) -> Option<(u64, usize)> {
        let name = OsStr::new(name);
        let modules: MinidumpModuleList = self.state.borrow().dump.get_stream().unwrap();

        for module in modules.iter() {
            let path = std::path::Path::new(&module.name);

            if path.file_name() == Some(&name) {
                let size = usize::try_from(module.raw.size_of_image).unwrap();
                return Some((module.raw.base_of_image, size));
            }
        }

        None
    }

    // before accessing memory we need to make sure we've mapped it into the VM's memory space
    fn ensure_mapped(emu: &mut UnicornHandle, state: &mut ByondState, ptr: u64, size: usize) -> bool {
        let mut is_mapped = false;

        let pages: MinidumpMemoryList = state.dump.get_stream().unwrap();

        for page in pages.iter() {
            if ptr < (page.base_address + page.size) && page.base_address < (ptr + size as u64) {
                // TODO: Check that we have mapped multiple pages if necessary?
                is_mapped = true;

                // The page has already been mapped
                if state.mapped.contains(&page.base_address) {
                    continue;
                }
                state.mapped.insert(page.base_address);

                emu.mem_map(page.base_address, page.size as usize, Permission::ALL)
                    .unwrap();

                emu.mem_write(page.base_address, page.bytes).unwrap();
            }
        }

        is_mapped
    }
}

fn find_byond_core<T>(dump: &Minidump<T>) -> Option<(u64, usize)>
where
    T: Deref<Target = [u8]>,
{
    let modules: MinidumpModuleList = dump.get_stream().unwrap();

    for module in modules.iter() {
        let path = std::path::Path::new(&module.name);

        if path.file_name() == Some(&OsStr::new("byondcore.dll")) {
            return Some((module.raw.base_of_image, module.raw.size_of_image as usize));
        }
    }

    None
}

fn try_map(emu: &mut UnicornHandle, pages: &MinidumpMemoryList, ptr: u64, size: usize) -> bool {
    let target_range = ptr..(ptr + size as u64);
    let mut did_map = false;

    for page in pages.iter() {
        let range = page.base_address..(page.base_address + page.size);
        if target_range.start < range.end && range.start < target_range.end {
            // TODO: don't map twice!
            let _ = emu.mem_map(page.base_address, page.size as usize, Permission::ALL);
            let _ = emu.mem_write(page.base_address, page.bytes);

            did_map = true;
        }
    }

    did_map
}

fn main() {
    let mut dump = ByondEmulator::new(Path::new("E:/dmdiag/tgstation.dmp"));

    //
    // StackOp::push(&0x00000001, &mut emu); // Unimportant flag params for get_string_id
    // StackOp::push(&0x00000000, &mut emu); // Unimportant flag params for get_string_id
    // StackOp::push(&0x00000000, &mut emu); // Unimportant flag params for get_string_id
    // StackOp::push(&0x80000100, &mut emu); // String ptr
    // StackOp::push(&0x00000000, &mut emu); // Return address (we'll handle the crash)
    //
    // let _ = emu.emu_start(get_string_id, 0, 10 * SECOND_SCALE, 16000);
    //

    // Setup stack for call
    dump.stack_push(value::Value {
        kind: 0x02,
        data: 0x1401,
    }); // Reference of my testing datum
    dump.stack_push(0x00000000); // Return address (we'll handle the crash)

    let functions = dump.state.borrow().functions.clone();

    let mut emu = dump.unicorn.borrow();
    let _ = emu.emu_start(
        functions.to_string.unwrap(),
        0,
        10 * SECOND_SCALE,
        16000,
    );
    let string_index = emu.reg_read(RegisterX86::EAX as i32).unwrap();

    println!("String index = {:x}", string_index);

    dump.stack_push(string_index as u32);
    dump.stack_push(0x00000000); // Return address (we'll handle the crash)

    let mut emu = dump.unicorn.borrow();
    let _ = emu.emu_start(
        functions.get_string_table_entry.unwrap(),
        0,
        10 * SECOND_SCALE,
        16000,
    );

    let ptr = emu.reg_read(RegisterX86::EAX as i32).unwrap();
    ByondEmulator::ensure_mapped(&mut emu, &mut dump.state.borrow_mut(), ptr, size_of::<byond::StringEntry>());
    let mut emu = dump.unicorn.borrow();
    let bytes = emu
        .mem_read_as_vec(ptr, size_of::<byond::StringEntry>())
        .unwrap();

    let mut string_ptr = unsafe {
        let string_entry: *const byond::StringEntry = bytes.as_ptr() as *const _;
        println!("{:x?}", *string_entry);
        (*string_entry).data as u64
    };

    println!("String ptr = {:x}", string_ptr);

    let mut characters: Vec<u8> = vec![];

    let state = dump.state.borrow();
    let pages: MinidumpMemoryList = state.dump.get_stream().unwrap();

    loop {
        let mut char: [u8; 1] = [0];
        try_map(&mut emu, &pages, string_ptr, 1);
        emu.mem_read(string_ptr, &mut char).unwrap();

        if char[0] == 0 {
            break;
        }

        characters.push(char[0]);
        string_ptr += 1;
    }

    println!("GOT STRING! {:?}", String::from_utf8_lossy(&characters));
}