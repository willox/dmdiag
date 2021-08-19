use crate::stack_op::{StackOp};
use unicorn::UnicornHandle;

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

        Value { kind: kind as u8, data }
    }
}