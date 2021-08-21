use crate::stack_op::StackOp;
use unicorn::UnicornHandle;

#[repr(C)]
#[derive(Debug, Clone)]
pub struct StringId(pub u32);

#[repr(C)]
#[derive(Debug, Clone)]
pub struct StringEntry {
    pub data: u32,
    pub this: StringId,
    pub left: u32,
    pub right: u32,
    pub ref_count: u32,
    pub unk_1: u32,
    pub unk_2: u32, // Length? This will help.
}

#[repr(C)]
#[derive(Debug)]
pub struct Value {
    pub kind: u8,
    pub data: u32,
}

impl StackOp for Value {
    fn push(&self, emu: &mut UnicornHandle) {
        StackOp::push(&self.data, emu);
        StackOp::push(&(self.kind as u32), emu);
    }

    fn pop(emu: &mut UnicornHandle) -> Self {
        let kind: u32 = StackOp::pop(emu);
        let data: u32 = StackOp::pop(emu);

        Value {
            kind: kind as u8,
            data,
        }
    }
}
