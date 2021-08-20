mod gdt;
mod sigscan;
mod stack_op;
mod value;

use std::ffi::OsStr;
use std::ops::Deref;

use unicorn::unicorn_const::{Arch, HookType, Mode, Permission, SECOND_SCALE};
use unicorn::{RegisterX86, UnicornHandle, X86Mmr};

use minidump::{Minidump, MinidumpMemory, MinidumpMemoryList, MinidumpModuleList};

use crate::stack_op::StackOp;
use crate::value::Value;

const STACK_ADDRESS: u64 = 0x80000000;
const STACK_SIZE: usize = 0x1000;

// GDT just lives at the beginning of the stack allocation. Pray we don't see any overflows.
const GDT_ADDRESS: u64 = STACK_ADDRESS;

const TIB_ADDRESS: u64 = 0x1000;
const TIB_SIZE: usize = 0x1000;

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
            println!("MAPPING {:X} -> {:x}", page.base_address, page.base_address + page.size);

            emu.mem_map(page.base_address, page.size as usize, Permission::ALL)
                .expect("failed to map page");
    
            emu.mem_write(page.base_address, page.bytes)
                .expect("failed to write page");

            did_map = true;
        }
    }

    did_map
}

fn main() {
    let mut unicorn = unicorn::Unicorn::new(Arch::X86, Mode::MODE_32)
        .expect("failed to initialize Unicorn instance");

    let mut emu = unicorn.borrow();

    let dump = minidump::Minidump::read_path("E:/dmdiag/tgstation.dmp").unwrap();
    let dump = Box::new(dump);
    let dump = Box::leak(dump);
    let pages: MinidumpMemoryList = dump.get_stream().unwrap();
    let pages = Box::new(pages);
    let pages = Box::leak(pages);

    // Find get_variable
    let (byond_core_start, byond_core_size) = find_byond_core(&dump).unwrap();

    assert!(try_map(&mut emu, &pages, byond_core_start, byond_core_size));

    let byond_core = emu
        .mem_read_as_vec(byond_core_start, byond_core_size)
        .unwrap();
    let get_variable = byond_core_start
        + sigscan::find(
            &byond_core,
            &[
                Some(0x55), Some(0x8B), Some(0xEC), Some(0x8B), Some(0x4D), None, Some(0x0F), Some(0xB6), Some(0xC1), Some(0x48), Some(0x83), Some(0xF8), None, Some(0x0F), Some(0x87), None, None, None, None, Some(0x0F), Some(0xB6), Some(0x80), None, None, None, None, Some(0xFF), Some(0x24), Some(0x85), None, None, None, None, Some(0xFF), Some(0x75), None, Some(0xFF), Some(0x75), None, Some(0xE8)
            ],
        )
        .unwrap() as u64;
    let get_string_id = byond_core_start
        + sigscan::find(
            &byond_core,
            &[
                Some(0x55), Some(0x8B), Some(0xEC), Some(0x8B), Some(0x45), None, Some(0x83), Some(0xEC), None, Some(0x53), Some(0x56), Some(0x8B), Some(0x35), None, None, None, None, Some(0x57), Some(0x85), Some(0xC0), Some(0x75), None, Some(0x68), None, None, None, None,
            ],
        )
        .unwrap() as u64;

    // emu.add_mem_hook(HookType::MEM_ALL, 0, 0xFFFFFFFFFFFFFFFF, |x, a, b, c, d| {
    //     println!("HOOK! {:?}, {:x}, {}, {}", a, b, c, d);
    // })
    // .unwrap();

    emu.add_mem_invalid_hook(HookType::MEM_INVALID, 0, 0xFFFFFFFFFFFFFFFF, move |mut x, a, b, c, d| {
        try_map(&mut x, &pages, b, c)
    })
    .unwrap();

    emu.add_code_hook(0, 0xFFFFFFFFFFFFFFFF, |emu, adr, size| {
        let eax = emu.reg_read(RegisterX86::EAX as i32).unwrap();
        let edx = emu.reg_read(RegisterX86::EDX as i32).unwrap();
        let esp = emu.reg_read(RegisterX86::ESP as i32).unwrap();
        let ebp = emu.reg_read(RegisterX86::EBP as i32).unwrap();
        let fs = emu.reg_read(RegisterX86::FS as i32).unwrap();
        println!(
            "HOOK! {:x}, {:x} [eax = {:x}, edx= {:x}, esp = {:x}, ebp = {:x}, fs = {:x}]",
            adr, size, eax, edx, esp, ebp, fs
        );
    })
    .unwrap();



    // for page in pages.iter() {
    //     println!("Mapping {:x} {:X}", page.base_address, page.size);
    //     emu.mem_map(page.base_address, page.size as usize, Permission::ALL)
    //         .expect("failed to map page");
// 
    //     emu.mem_write(page.base_address, page.bytes)
    //         .expect("failed to write page");
    // }

    //
    // Stack
    //
    emu.mem_map(STACK_ADDRESS, STACK_SIZE, Permission::ALL)
        .expect("failed to allocate stack");

    emu.reg_write(RegisterX86::ESP as i32, STACK_ADDRESS + STACK_SIZE as u64).unwrap();

    //
    // GDT
    //
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

    //
    // TIB
    //
    emu.mem_map(TIB_ADDRESS, TIB_SIZE, Permission::ALL)
        .expect("failed to allocate tib");
    emu.mem_write(TIB_ADDRESS + 0x18, &(TIB_ADDRESS as u32).to_le_bytes())
        .unwrap();

    emu.mem_write(0x80000100, b"pparent_type\0")
        .unwrap();
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
    StackOp::push(&0x00000082, &mut emu);   // String id for `parent_type`
    StackOp::push(&value::Value {
        kind: 0x02,
        data: 0x1401,
    }, &mut emu);                           // Reference of my testing datum
    StackOp::push(&0x00000000, &mut emu);   // Return address (we'll handle the crash)
    let _ = emu.emu_start(get_variable, 0, 10 * SECOND_SCALE, 16000);

    let eax = emu.reg_read(RegisterX86::EAX as i32).unwrap();
    let edx = emu.reg_read(RegisterX86::EDX as i32).unwrap();

    println!("{:x?} {:x?}", eax, edx);
}



