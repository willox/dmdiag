#[repr(C)]
#[derive(Debug)]
pub struct StringEntry {
    pub data: u32,
    pub this: u32,
    pub left: u32,
    pub right: u32,
    pub ref_count: u32,
    pub unk_1: u32,
    pub unk_2: u32, // Length? This will help.
}
