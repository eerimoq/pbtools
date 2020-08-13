enum WireType {
    Varint = 0,
    // Bits64 = 1,
    LengthDelimited = 2,
    // Bits32 = 5
}

pub struct Encoder {
    pub encoded: Vec<u8>
}

impl Encoder {
    fn write_tagged_varint(&mut self,
                           field_number: u32,
                           wire_type: WireType,
                           value: u64) {
        if value > 0 {
            self.write_tag(field_number, wire_type);
            self.write_varint(value);
        }
    }

    fn write_tag(&mut self, field_number: u32, wire_type: WireType) {
        self.write_varint((field_number << 3 | wire_type as u32) as u64);
    }

    fn write_varint(&mut self, mut value: u64) {
        loop {
            let item = value as u8;
            value >>= 7;

            if value == 0 {
                self.encoded.push(item);
                break;
            } else {
                self.encoded.push(item | 0x80);
            }
        }
    }

    pub fn write_length_delimited(&mut self, field_number: u32, value: u64) {
        self.write_tag(field_number, WireType::LengthDelimited);
        self.write_varint(value);
    }

    pub fn write_string(&mut self, field_number: u32, value: &String) {
        self.write_tagged_varint(field_number,
                                 WireType::LengthDelimited,
                                 value.len() as u64);
        self.encoded.extend(value.as_bytes());
    }

    pub fn write_int32(&mut self, field_number: u32, value: i32) {
        self.write_tagged_varint(field_number,
                                 WireType::Varint,
                                 value as i64 as u64);
    }
}

pub struct Decoder {
    pub pos: usize,
    pub encoded: Vec<u8>
}

impl Decoder {
    pub fn available(&self) -> bool {
        self.pos < self.encoded.len()
    }

    pub fn read_tag(&mut self) -> (u32, u32) {
        if self.pos < self.encoded.len() {
            self.pos += 1;
            (1, self.encoded[self.pos - 1] as u32)
        } else {
            (0, 0)
        }
    }

    pub fn read_string(&mut self, _wire_type: u32) -> String {
        String::from("koko")
    }

    pub fn read_int32(&mut self, _wire_type: u32) -> i32 {
        5
    }
}
