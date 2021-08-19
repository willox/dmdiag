#[macro_export]
macro_rules! signature {
    ($sig:tt) => {
        sigscan::convert_signature!($sig)
    };
}

pub fn find(target: &[u8], signature: &[Option<u8>]) -> Option<usize> {
    let mut cursor = 0;
    let mut signature_cursor = 0;
    let mut result: Option<usize> = None;

    while cursor < target.len() {
        if signature[signature_cursor] == None
            || signature[signature_cursor] == Some(target[cursor])
        {
            if signature.len() <= signature_cursor + 1 {
                if result.is_some() {
                    // Found two matches.
                    return None;
                }
                result = Some(cursor - signature_cursor);
                cursor -= signature_cursor;
                signature_cursor = 0;
            } else {
                signature_cursor += 1;
            }
        } else {
            cursor -= signature_cursor;
            signature_cursor = 0;
        }
        cursor += 1;
    }

    result
}
