use crate::{sigscan, ByondEmulator};

#[derive(Clone, Default)]
pub struct Functions {
    // (Value, string id) -> Value
    pub get_variable: Option<u64>,

    // (char*, 0, 0, 1) -> string id
    pub get_string_id: Option<u64>,

    // (Value) -> string id
    pub to_string: Option<u64>,

    // (string id) -> byond::StringEntry
    pub get_string_table_entry: Option<u64>,

    // (Value list, Value index) -> Value
    pub get_assoc_element: Option<u64>,
}

impl Functions {
    pub(crate) fn new(dump: &mut ByondEmulator) -> Self {
        // Find get_variable
        let (byond_core_start, byond_core_size) = dump.find_module("byondcore.dll").unwrap();

        let mut emu = &mut dump.unicorn;
        ByondEmulator::assert_mapped(
            &mut emu,
            &mut dump.state.borrow_mut(),
            byond_core_start,
            byond_core_size
        );

        let byond_core = emu
            .mem_read_as_vec(byond_core_start, byond_core_size)
            .unwrap();

        let get_variable = sigscan::find(
            &byond_core,
            &[
                Some(0x55),
                Some(0x8B),
                Some(0xEC),
                Some(0x8B),
                Some(0x4D),
                None,
                Some(0x0F),
                Some(0xB6),
                Some(0xC1),
                Some(0x48),
                Some(0x83),
                Some(0xF8),
                None,
                Some(0x0F),
                Some(0x87),
                None,
                None,
                None,
                None,
                Some(0x0F),
                Some(0xB6),
                Some(0x80),
                None,
                None,
                None,
                None,
                Some(0xFF),
                Some(0x24),
                Some(0x85),
                None,
                None,
                None,
                None,
                Some(0xFF),
                Some(0x75),
                None,
                Some(0xFF),
                Some(0x75),
                None,
                Some(0xE8),
            ],
        )
        .map(|x| byond_core_start + x as u64);
        let get_string_id = sigscan::find(
            &byond_core,
            &[
                Some(0x55),
                Some(0x8B),
                Some(0xEC),
                Some(0x8B),
                Some(0x45),
                None,
                Some(0x83),
                Some(0xEC),
                None,
                Some(0x53),
                Some(0x56),
                Some(0x8B),
                Some(0x35),
                None,
                None,
                None,
                None,
                Some(0x57),
                Some(0x85),
                Some(0xC0),
                Some(0x75),
                None,
                Some(0x68),
                None,
                None,
                None,
                None,
            ],
        )
        .map(|x| byond_core_start + x as u64);
        let to_string = sigscan::find(
            &byond_core,
            &[
                Some(0x55),
                Some(0x8B),
                Some(0xEC),
                Some(0x6A),
                Some(0xFF),
                Some(0x68),
                None,
                None,
                None,
                None,
                Some(0x64),
                Some(0xA1),
                None,
                None,
                None,
                None,
                Some(0x50),
                Some(0x83),
                Some(0xEC),
                Some(0x18),
                Some(0x53),
                Some(0x56),
                Some(0x57),
                Some(0xA1),
                None,
                None,
                None,
                None,
                Some(0x33),
                Some(0xC5),
                Some(0x50),
                Some(0x8D),
                Some(0x45),
                None,
                Some(0x64),
                Some(0xA3),
                None,
                None,
                None,
                None,
                Some(0x8B),
                Some(0x4D),
                None,
                Some(0x0F),
                Some(0xB6),
                Some(0xC1),
            ],
        )
        .map(|x| byond_core_start + x as u64);
        let get_string_table_entry = sigscan::find(
            &byond_core,
            &[
                Some(0x55),
                Some(0x8B),
                Some(0xEC),
                Some(0x8B),
                Some(0x4D),
                Some(0x08),
                Some(0x3B),
                Some(0x0D),
                None,
                None,
                None,
                None,
                Some(0x73),
                Some(0x10),
                Some(0xA1),
            ],
        )
        .map(|x| byond_core_start + x as u64);

        let get_assoc_element = sigscan::find(
            &byond_core,
            &[
                Some(0x55),
                Some(0x8B),
                Some(0xEC),
                Some(0x51),
                Some(0x8B),
                Some(0x4D),
                Some(0x08),
                Some(0xC6),
                Some(0x45),
                Some(0xFF),
                Some(0x00),
                Some(0x80),
                Some(0xF9),
                Some(0x05),
                Some(0x76),
                Some(0x11),
                Some(0x80),
                Some(0xF9),
                Some(0x21),
                Some(0x74),
                Some(0x10),
                Some(0x80),
                Some(0xF9),
                Some(0x0D),
                Some(0x74),
                Some(0x0B),
                Some(0x80),
                Some(0xF9),
                Some(0x0E),
                Some(0x75),
                Some(0x65),
                Some(0xEB),
                Some(0x04),
                Some(0x84),
                Some(0xC9),
                Some(0x74),
                Some(0x5F),
                Some(0x6A),
                Some(0x00),
                Some(0x8D),
                Some(0x45),
                Some(0xFF),
                Some(0x50),
                Some(0xFF),
                Some(0x75),
                Some(0x0C),
                Some(0x51),
                Some(0x6A),
                Some(0x00),
                Some(0x6A),
                Some(0x7B)
            ],
        )
        .map(|x| byond_core_start + x as u64);

        Functions {
            get_variable,
            get_string_id,
            to_string,
            get_string_table_entry,
            get_assoc_element,
        }
    }
}
