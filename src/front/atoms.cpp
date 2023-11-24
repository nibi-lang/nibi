#include "atoms.hpp"
#include "atom_view.hpp"
#include "util.hpp"
#include <limits>
/*
    Atom binary encoding :

    TOP-LEVEL list:

    |     HEADER                      |   DATA       |
    | 1 byte | 8 bytes                | ... variable |
      LIST     First instruction POS

               ^ 0xFF 0xFF 0xFF 0xFF
                 0xFF 0xFF 0xFF 0xFF iff empty list

    CONTAINED lists:

    |    HEADER        |    DATA
    | 1 byte | 8 bytes | ... variable |
      ID         POS

        SYMBOL:
          | 2 bytes | <LEN> bytes |
            LEN       DATA

        INTEGER:
          | 8 bytes |
            value

        REAL
          | 8 bytes |
           value

        STRING:
          | 2 bytes | <LEN> bytes |
            LEN       DATA

        LIST:
          | 2 bytes | <LEN> bytes |
            LEN       DATA

*/

void file_position_s::encode_to(std::vector<uint8_t>& data) const {
  util::pack_into<uint32_t>((uint32_t)this->line, data);
  util::pack_into<uint32_t>((uint32_t)this->col, data);
}

void atom_c::encode_base(std::vector<uint8_t>& data) const {
  data.push_back((uint8_t)this->type);
  this->pos.encode_to(data);
}

void atom_symbol_c::encode_to(std::vector<uint8_t>& data) const {
  encode_base(data);
  util::pack_into<uint16_t>((uint16_t)this->data.size(), data);
  util::pack_string_into(this->data, data); 
}

void atom_int_c::encode_to(std::vector<uint8_t>& data) const {
  encode_base(data);
  util::pack_into<int64_t>(this->data, data);
}

void atom_real_c::encode_to(std::vector<uint8_t>& data) const {
  encode_base(data);
  util::pack_into<uint64_t>(
    util::real_to_uint64_t(this->data), data);
}

void atom_string_c::encode_to(std::vector<uint8_t>& data) const {
  encode_base(data);
  encode_base(data);
  util::pack_into<uint16_t>((uint16_t)this->data.size(), data);
  util::pack_string_into(this->data, data); 
}

void atom_list_c::encode_to(std::vector<uint8_t>& data) const {
  encode_base(data);

  // Don't include header as we've written our own
  encode_atom_list(this->data, data, false);
}

void encode_atom_list(
    const atom_list_t& list,
    std::vector<uint8_t>& data,
    const bool include_header) {

  if (include_header) {
    data.push_back((uint8_t)atom_type_e::LIST);
    if (list.empty()) {
      util::pack_into<uint32_t>((uint32_t)std::numeric_limits<uint32_t>::max(), data);
      util::pack_into<uint32_t>((uint32_t)std::numeric_limits<uint32_t>::max(), data);
    } else {
      list[0]->pos.encode_to(data);
    }
  }

  std::vector<uint8_t> encoded_list;
  for(auto& atom : list) {
    atom->encode_to(encoded_list);
  }
  util::pack_into<uint16_t>((uint16_t)encoded_list.size(), data);
  util::merge_vecs(data, encoded_list);
}


