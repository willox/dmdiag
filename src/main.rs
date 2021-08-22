mod functions;
mod gdt;
mod sigscan;
mod stack_op;
mod structures;

use std::cell::RefCell;
use std::collections::{BTreeMap, HashMap, HashSet};
use std::convert::TryFrom;
use std::ffi::OsStr;
use std::iter::Map;
use std::mem::size_of;
use std::ops::Deref;
use std::path::Path;
use std::rc::Rc;

use structures::{StringEntry, StringId};
use unicorn::unicorn_const::{Arch, HookType, Mode, Permission, SECOND_SCALE};
use unicorn::{RegisterX86, Unicorn, UnicornHandle, X86Mmr};

use minidump::{Minidump, MinidumpMemory, MinidumpMemoryList, MinidumpModuleList};

use crate::stack_op::StackOp;
use crate::structures::Value;

// TODO: dynamic allocations for all of these
const STACK_ADDRESS: u64 = 0x80000000;
const STACK_SIZE: usize = 0x100000;
const GDT_ADDRESS: u64 = STACK_ADDRESS;

struct ByondEmulator {
    unicorn: Unicorn,

    // addresses of byond functions
    functions: functions::Functions,

    // state that is shared with our unicorn hook functions
    state: Rc<RefCell<ByondState>>,
}

struct ByondState {
    dump: minidump::Minidump<'static, minidump::Mmap>, // 🤔

    // base addresses of memory regions that have already been mapped in
    mapped: HashSet<u64>,
}

impl ByondEmulator {
    fn new(path: &Path) -> Self {
        let unicorn = unicorn::Unicorn::new(Arch::X86, Mode::MODE_32)
            .expect("failed to initialize Unicorn instance");

        let dump = minidump::Minidump::read_path(path).expect("failed to load dump");

        let mut this = Self {
            unicorn,
            functions: Default::default(),
            state: Rc::new(RefCell::new(ByondState {
                dump,
                mapped: HashSet::new(),
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

            // Grab TIB ptr
            let tib = {
                let mut emu = this.unicorn.borrow();

                let mut state = this.state.borrow_mut();
                let threads: minidump::MinidumpThreadList = state.dump.get_stream().unwrap();
                let thread = &threads.threads[0];

                let tib64 = thread.raw.teb;
                Self::ensure_mapped(&mut emu, &mut state, tib64, 4);

                let mut bytes: [u8; 4] = [0, 0, 0, 0];
                emu.mem_read(tib64, &mut bytes).unwrap();
                u32::from_le_bytes(bytes) as u64
            };

            // GDT (poor man's version)
            {
                let mut emu = this.unicorn.borrow();
                let gdt = gdt::create_basic_gdt(tib);
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

            {
                let mut emu = this.unicorn.borrow();

                emu.add_insn_invalid_hook(|mut emu| {
                    let rip = emu.reg_read(RegisterX86::EIP as i32).unwrap();
                    let code = emu.mem_read_as_vec(rip, 2).unwrap(); // TODO: check mapped

                    // Fully functional RDRAND
                    if code == &[0x0F, 0xC7] {
                        emu.reg_write(RegisterX86::EIP as i32, rip + 3).unwrap();
                        let eflags = emu.reg_read(RegisterX86::EFLAGS as i32).unwrap();
                        let eflags = (eflags & !0x8D4) | 0x1;
                        emu.reg_write(RegisterX86::EFLAGS as i32, eflags).unwrap();
                        return true;
                    }

                    false
                })
                .unwrap();

                let formatter = zydis::Formatter::new(zydis::FormatterStyle::INTEL).unwrap();
                let decoder = zydis::Decoder::new(
                    zydis::MachineMode::LONG_COMPAT_32,
                    zydis::AddressWidth::_32,
                )
                .unwrap();

                emu.add_code_hook(0, 0xFFFFFFFFFFFFFFFF, move |emu, ptr, len| {
                    if len == 0xf1f1f1f1 {
                        return;
                    }

                    let code = emu.mem_read_as_vec(ptr, len as usize).unwrap();

                    let mut buffer = [0u8; 200];
                    let mut buffer = zydis::OutputBuffer::new(&mut buffer);

                    let instruction = decoder.decode(&code).unwrap().unwrap();

                    formatter
                        .format_instruction(&instruction, &mut buffer, Some(ptr), None)
                        .unwrap();

                    let mut registers: BTreeMap<&'static str, i32> = BTreeMap::new();

                    let mut write_register = |register: zydis::Register| {
                        use zydis::Register;

                        match register {
                            Register::NONE => {}
                            Register::AH | Register::AL | Register::AX | Register::EAX => {
                                let eax = emu.reg_read_i32(RegisterX86::EAX as i32).unwrap();
                                registers.insert("eax", eax);
                            }
                            Register::CH | Register::CL | Register::CX | Register::ECX => {
                                let ecx = emu.reg_read_i32(RegisterX86::ECX as i32).unwrap();
                                registers.insert("ecx", ecx);
                            }
                            Register::DH | Register::DL | Register::DX | Register::EDX => {
                                let edx = emu.reg_read_i32(RegisterX86::EDX as i32).unwrap();
                                registers.insert("edx", edx);
                            }
                            Register::BH | Register::BL | Register::BX | Register::EBX => {
                                let ebx = emu.reg_read_i32(RegisterX86::EBX as i32).unwrap();
                                registers.insert("ebx", ebx);
                            }
                            Register::SPL | Register::SP | Register::ESP => {
                                let esp = emu.reg_read_i32(RegisterX86::ESP as i32).unwrap();
                                registers.insert("esp", esp);
                            }
                            Register::BPL | Register::BP | Register::EBP => {
                                let ebp = emu.reg_read_i32(RegisterX86::EBP as i32).unwrap();
                                registers.insert("ebp", ebp);
                            }
                            Register::SIL | Register::SI | Register::ESI => {
                                let esi = emu.reg_read_i32(RegisterX86::ESI as i32).unwrap();
                                registers.insert("esi", esi);
                            }
                            Register::DIL | Register::DI | Register::EDI => {
                                let edi = emu.reg_read_i32(RegisterX86::EDI as i32).unwrap();
                                registers.insert("edi", edi);
                            }
                            _ => {}
                        }
                    };

                    for operand in &instruction.operands[0..instruction.operand_count as usize] {
                        if operand.visibility == zydis::OperandVisibility::HIDDEN {
                            continue;
                        }

                        if !operand.action.intersects(zydis::OperandAction::MASK_READ) {
                            continue;
                        }

                        match operand.ty {
                            zydis::OperandType::REGISTER => {
                                write_register(operand.reg);
                            }

                            zydis::OperandType::MEMORY => {
                                write_register(operand.mem.base);
                                write_register(operand.mem.index);
                            }

                            _ => {}
                        }
                    }

                    let mut register_text = String::new();

                    if !registers.is_empty() {
                        for (name, value) in registers {
                            register_text.push_str(&format!("{} = {:08X}, ", name, value));
                        }

                        register_text = (&register_text[..register_text.len() - 2]).to_owned();
                    }

                    println!(
                        "{:08X}    {: <48} {:}",
                        ptr,
                        buffer.to_string(),
                        register_text
                    );
                })
                .unwrap();

                emu.add_mem_hook(
                    HookType::MEM_VALID,
                    0x00000000,
                    0xFFFFFFFF,
                    |a, b, c, d, e| {
                        // println!("READ {:x} -> {:x}", c, c + d as u64);
                    },
                )
                .unwrap();

                let state = Rc::clone(&this.state);
                emu.add_mem_invalid_hook(
                    HookType::MEM_INVALID,
                    0,
                    0xFFFFFFFFFFFFFFFF,
                    move |mut emu, a, b, c, d| {
                        Self::ensure_mapped(&mut emu, &mut state.borrow_mut(), b, c)
                    },
                )
                .unwrap();
            }

            // Kind of... bad
            this.functions = functions::Functions::new(&mut this);
        }

        this
    }

    fn stack_clear(&mut self) {
        let mut emu = self.unicorn.borrow();

        // (≻‿≺ 乂 da
        // 乂 ≻‿≺) da
        // (≻‿≺ 乂 da
        // 乂 ≻‿≺) da
        emu.mem_write(STACK_ADDRESS + 0x100, &[0xda; STACK_SIZE - 0x100])
            .unwrap();

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
    fn ensure_mapped(
        emu: &mut UnicornHandle,
        state: &mut ByondState,
        ptr: u64,
        size: usize,
    ) -> bool {
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

    fn get_string_table_entry(&mut self, id: StringId) -> StringEntry {
        self.stack_clear();
        self.stack_push(id.0);
        self.stack_push(0x00000000); // Return address (we'll handle the crash)

        let mut emu = self.unicorn.borrow();

        // TODO: Actually check for success
        let _ = emu.emu_start(self.functions.get_string_table_entry.unwrap(), 0, 0, 16000);

        let ptr = emu.reg_read(RegisterX86::EAX as i32).unwrap();
        ByondEmulator::ensure_mapped(
            &mut emu,
            &mut self.state.borrow_mut(),
            ptr,
            size_of::<structures::StringEntry>(),
        );

        let bytes = emu
            .mem_read_as_vec(ptr, size_of::<structures::StringEntry>())
            .unwrap();

        // TODO: not good
        unsafe {
            let string_entry: *const structures::StringEntry = bytes.as_ptr() as *const _;
            (*string_entry).clone()
        }
    }

    fn read_null_terminated_string(&mut self, ptr: u32) -> String {
        let mut ptr = ptr as u64;
        let mut characters: Vec<u8> = vec![];

        let mut emu = self.unicorn.borrow();

        loop {
            let mut char: [u8; 1] = [0];
            assert!(ByondEmulator::ensure_mapped(
                &mut emu,
                &mut self.state.borrow_mut(),
                ptr,
                1
            ));
            emu.mem_read(ptr, &mut char).unwrap();

            if char[0] == 0 {
                break;
            }

            characters.push(char[0]);
            ptr += 1;
        }

        String::from_utf8_lossy(&characters).to_string()
    }

    fn get_string_id(&mut self, string: &str) -> StringId {
        self.stack_clear();
        self.stack_push(1);
        self.stack_push(0);
        self.stack_push(0);
        self.stack_push(STACK_ADDRESS as u32 + 0x100);
        self.stack_push(0x00000000); // Return address (we'll handle the crash)

        let mut emu = self.unicorn.borrow();
        emu.mem_write(STACK_ADDRESS + 0x100, string.as_bytes())
            .unwrap();
        emu.mem_write(STACK_ADDRESS + 0x100 + string.len() as u64, &[0])
            .unwrap();

        // TODO: Actually check for success
        let _ = emu.emu_start(self.functions.get_string_id.unwrap(), 0, 0, 160000);

        StringId(emu.reg_read(RegisterX86::EAX as i32).unwrap() as u32)
    }

    fn to_string(&mut self, val: Value) -> String {
        self.stack_clear();
        self.stack_push(val);
        self.stack_push(0x00000000); // Return address (we'll handle the crash)

        let mut emu = self.unicorn.borrow();

        // TODO: Actually check for success
        let _ = emu.emu_start(self.functions.to_string.unwrap(), 0, 0, 16000);

        let id = StringId(emu.reg_read(RegisterX86::EAX as i32).unwrap() as u32);

        let string_table_entry = self.get_string_table_entry(id);
        self.read_null_terminated_string(string_table_entry.data)
    }

    fn get_field(&mut self, val: Value, field: &str) -> Value {
        let field = self.get_string_id(field);

        self.stack_clear();
        self.stack_push(field.0);
        self.stack_push(val);
        self.stack_push(0x00000000); // Return address (we'll handle the crash)

        let mut emu = self.unicorn.borrow();

        // TODO: Actually check for success
        let _ = emu
            .emu_start(self.functions.get_variable.unwrap(), 0, 0, 1600000)
            .unwrap();

        let eax = emu.reg_read(RegisterX86::EAX as i32).unwrap() as u32;
        let edx = emu.reg_read(RegisterX86::EDX as i32).unwrap() as u32;

        Value {
            kind: eax as u8,
            data: edx,
        }
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

    let mannitol_pill = Value {
        kind: 0x02,
        data: 0x1423,
    };

    // println!("manitol_pill = {:?}", dump.to_string(mannitol_pill));
    let mannitol_pill_loc = dump.get_field(mannitol_pill, "loccywocky");
    println!("manitol_pill.loc = {:?}", mannitol_pill_loc);
}
