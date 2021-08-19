// https://scoding.de/setting-global-descriptor-table-unicorn

const F_GRANULARITY: u8 = 0x8; // If set block=4KiB otherwise block=1B
const F_PROT_32: u8 = 0x4; // Protected Mode 32 bit
const F_LONG: u8 = 0x2; // Long Mode
const F_AVAILABLE: u8 = 0x1; // Free Use
const A_PRESENT: u8 = 0x80; // Segment active
const A_PRIV_3: u8 = 0x60; // Ring 3 Privs
const A_PRIV_2: u8 = 0x40; // Ring 2 Privs
const A_PRIV_1: u8 = 0x20; // Ring 1 Privs
const A_PRIV_0: u8 = 0x0; // Ring 0 Privs
const A_CODE: u8 = 0x10; // Code Segment
const A_DATA: u8 = 0x10; // Data Segment
const A_TSS: u8 = 0x0; // TSS
const A_GATE: u8 = 0x0; // GATE
const A_EXEC: u8 = 0x8; // Executable
const A_DATA_WRITABLE: u8 = 0x2;
const A_CODE_READABLE: u8 = 0x2;
const A_DIR_CON_BIT: u8 = 0x4;
const S_GDT: u8 = 0x0; // Index points to GDT
const S_LDT: u8 = 0x4; // Index points to LDT
const S_PRIV_3: u8 = 0x3; // Ring 3 Privs
const S_PRIV_2: u8 = 0x2; // Ring 2 Privs
const S_PRIV_1: u8 = 0x1; // Ring 1 Privs
const S_PRIV_0: u8 = 0x0; // Ring 0 Privs

const GDT_ENTRY_SIZE: usize = 0x8;
const GDT_COUNT: usize = 5;

pub fn create_selector(idx: u8, flags: u8) -> u8 {
    let mut to_ret = flags;
    to_ret |= idx << 3;
    to_ret
}

type GdtEntry = [u8; GDT_ENTRY_SIZE];

fn create_gdt_entry(base: u64, limit: u64, access: u8, flags: u8) -> GdtEntry {
    let access = access as u64;
    let flags = flags as u64;
    let mut to_ret = limit & 0xffff;
    to_ret |= (base & 0xffffff) << 16;
    to_ret |= (access & 0xff) << 40;
    to_ret |= ((limit >> 16) & 0xf) << 48;
    to_ret |= (flags & 0xff) << 52;
    to_ret |= ((base >> 24) & 0xff) << 56;
    to_ret.to_le_bytes()
}

pub fn create_basic_gdt() -> [u8; GDT_ENTRY_SIZE * GDT_COUNT] {
    let mut gdt: [GdtEntry; GDT_COUNT] = [[0; GDT_ENTRY_SIZE]; GDT_COUNT];

    gdt[1] = create_gdt_entry(
        0x0,
        0xfffff000,
        A_PRESENT | A_DATA | A_DATA_WRITABLE | A_PRIV_0 | A_DIR_CON_BIT,
        F_PROT_32,
    );
    gdt[2] = create_gdt_entry(
        0x0,
        0xfffff000,
        A_PRESENT | A_DATA | A_DATA_WRITABLE | A_PRIV_0,
        F_PROT_32,
    );
    gdt[3] = create_gdt_entry(
        0x0,
        0xfffff000,
        A_PRESENT | A_DATA | A_DATA_WRITABLE | A_PRIV_3 | A_DIR_CON_BIT,
        F_PROT_32,
    );
    gdt[4] = create_gdt_entry(
        0x1000,
        0xfffff000,
        A_PRESENT | A_DATA | A_DATA_WRITABLE | A_PRIV_3 | A_DIR_CON_BIT,
        F_PROT_32,
    );

    unsafe { std::mem::transmute(gdt) }
}
