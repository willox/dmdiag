mod sigscan;
mod gdt;

use std::ffi::OsStr;
use std::ops::Deref;

use unicorn::unicorn_const::{Arch, HookType, Mode, Permission, SECOND_SCALE};
use unicorn::{RegisterX86, X86Mmr};

use minidump::{Minidump, MinidumpMemory, MinidumpMemoryList, MinidumpModuleList};

const STACK_ADDRESS: u64 = 0x80000000;
const STACK_SIZE: usize = 0x1000;

fn find_byond_core<T>(dump: &Minidump<T>) -> Option<(u64, usize)>
where
    T: Deref<Target = [u8]>,
{
    let modules: MinidumpModuleList = dump.get_stream().unwrap();

    for module in modules.iter() {
        let path = std::path::Path::new(&module.name);

        if path.file_name() == Some(&OsStr::new("byondcore.dll")) {
            return Some((module.raw.base_of_image, module.raw.size_of_image as usize))
        }
    }

    None
}

fn main() {
    let mut unicorn = unicorn::Unicorn::new(Arch::X86, Mode::MODE_32)
        .expect("failed to initialize Unicorn instance");

    let mut emu = unicorn.borrow();
    let dump = minidump::Minidump::read_path("E:/dmdiag/dreamdaemon.dmp").unwrap();
    let pages: MinidumpMemoryList = dump.get_stream().unwrap();

    for page in pages.iter() {
        emu.mem_map(page.base_address, page.size as usize, Permission::ALL)
            .expect("failed to map page");

        emu.mem_write(page.base_address, page.bytes)
            .expect("failed to write page");
    }

    let (byond_core_start, byond_core_size) = find_byond_core(&dump).unwrap();
    let byond_core = emu.mem_read_as_vec(byond_core_start, byond_core_size).unwrap();

    let get_variable = byond_core_start + sigscan::find(&byond_core, &[Some(0x55), Some(0x8B), Some(0xEC), Some(0x8B), Some(0x4D), None, Some(0x0F), Some(0xB6), Some(0xC1), Some(0x48), Some(0x83), Some(0xF8), None, Some(0x0F), Some(0x87), None, None, None, None, Some(0x0F), Some(0xB6), Some(0x80), None, None, None, None, Some(0xFF), Some(0x24), Some(0x85), None, None, None, None, Some(0xFF), Some(0x75), None, Some(0xFF), Some(0x75), None, Some(0xE8)]).unwrap() as u64;

    println!("get_variable = {:x}", get_variable);

    emu.mem_map(STACK_ADDRESS, STACK_SIZE, Permission::ALL)
        .expect("failed to allocate stack");

    emu.mem_write(STACK_ADDRESS + STACK_SIZE as u64 - 16, &[0, 0, 0, 0, 0x21, 0x00, 0x00, 0x00, 0, 0, 0, 0, 0, 0, 0, 0x25]);


    emu.add_code_hook(0, 0xFFFFFFFFFFFFFFFF, |emu, adr, size| {
        let eax = emu.reg_read(RegisterX86::EAX as i32).unwrap();
        let esp = emu.reg_read(RegisterX86::ESP as i32).unwrap();
        let ebp = emu.reg_read(RegisterX86::EBP as i32).unwrap();
        let fs = emu.reg_read(RegisterX86::FS as i32).unwrap();
        println!("HOOK! {:x}, {:x} [eax = {:x}, esp = {:x}, ebp = {:x}, fs = {:x}]", adr, size, eax, esp, ebp, fs);
    }).unwrap();

    emu.add_mem_hook(HookType::MEM_ALL, 0, 0xFFFFFFFFFFFFFFFF, |x, a, b, c, d| {
        println!("HOOK! {:?}, {:x}, {}, {}", a, b, c, d);
    }).unwrap();

    let gdt = gdt::create_basic_gdt();
    println!("setting gdt {:x?}", gdt);
    emu.mem_write(STACK_ADDRESS, &gdt).unwrap();
    
    let gdtr: X86Mmr = X86Mmr { selector: 0, base: STACK_ADDRESS, limit: 5 * 8, flags: 0 };
    unsafe {
        let gdtr = std::slice::from_raw_parts::<u8>(&gdtr as *const _ as *const u8, std::mem::size_of::<X86Mmr>());
        emu.reg_write_long(RegisterX86::GDTR as i32, gdtr.to_vec().into_boxed_slice()).unwrap();
    }

    let cs_ds_es = gdt::create_selector(3, 0x3);
    emu.reg_write(RegisterX86::CS as i32, cs_ds_es as u64).unwrap();
    emu.reg_write(RegisterX86::DS as i32, cs_ds_es as u64).unwrap();
    emu.reg_write(RegisterX86::ES as i32, cs_ds_es as u64).unwrap();

     let ss = gdt::create_selector(2, 0x0);
    emu.reg_write(RegisterX86::SS as i32, ss as u64).unwrap();

    let fs = gdt::create_selector(4, 0x3);
    emu.reg_write(RegisterX86::FS as i32, fs as u64).unwrap();

    emu.reg_write(RegisterX86::ESP as i32, STACK_ADDRESS + STACK_SIZE as u64 - 16).unwrap();
    emu.reg_write(RegisterX86::EBP as i32, 0).unwrap();

    println!("emu_start");    
    emu.emu_start(
        get_variable,
        0,
        10 * SECOND_SCALE,
        1600,
    ).unwrap();

    /*
    emu.reg_write(RegisterARM::R0 as i32, 122)
        .expect("failed write R0");
    emu.reg_write(RegisterARM::R5 as i32, 1337)
        .expect("failed write R5");

    let _ = emu.emu_start(
        0x1000,
        (0x1000 + arm_code32.len()) as u64,
        10 * SECOND_SCALE,
        1000,
    );
    assert_eq!(emu.reg_read(RegisterARM::R0 as i32), Ok(100));
    assert_eq!(emu.reg_read(RegisterARM::R5 as i32), Ok(1337));
    */

    println!("it works?");
}
