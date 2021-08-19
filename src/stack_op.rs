use unicorn::{RegisterX86, UnicornHandle};

pub trait StackOp {
    fn push(&self, emu: &mut UnicornHandle);
    fn pop(emu: &mut UnicornHandle) -> Self;
}

impl StackOp for u32 {
    fn push(&self, emu: &mut UnicornHandle) {
        let esp = emu
            .reg_read(RegisterX86::ESP as i32)
            .expect("failed to read esp")
            .checked_sub(0x4)
            .expect("esp underflow");

        emu.reg_write(RegisterX86::ESP as i32, esp)
            .expect("failed to write esp");

        emu.mem_write(esp, &self.to_le_bytes())
            .expect("stack write failure");
    }

    fn pop(emu: &mut UnicornHandle) -> Self {
        let esp = emu
            .reg_read(RegisterX86::ESP as i32)
            .expect("failed to read esp");

        let mut bytes: [u8; 4] = [0; 4];
        emu.mem_read(esp, &mut bytes).expect("stack read failure");

        emu.reg_write(
            RegisterX86::ESP as i32,
            esp.checked_add(0x4).expect("esp overflow"),
        )
        .expect("failed to write esp");

        Self::from_le_bytes(bytes)
    }
}
