#include "encoder.hpp"

#include <fmt/format.h>

struct data_s {
  bytes_t bytes;
  generator_error_cb err_cb;
};

#define ENCODE(to_, from_) \
  to_.insert(to_.end(), from_.begin(), from_.end());

void encode_atom(atom_ptr& atom, data_s &data) {

    // TODO: Check for macro, and handle specifically
    
    // NOTE:
    //       We COULD do this all in the atomiser instead
    //       of collecting unique pointers, shipping them
    //       here, and then deconstructing them, but since
    //       the machine is being designed as its written
    //       I figured it would be easier to segregate the
    //       steps.

    //       We could also change this to look for builtins
    //       and encode some more specific instructions, 
    //       or instead of symbol we could encode an ID
    //       into an array that points to how to handle
    //       that particular function
    //
    //

    switch(atom->type) {
      case atom_type_e::SYMBOL: {


        // Check if symbol is a builtin, otherwise we 
        // do the following ::
        data.bytes.push_back((uint8_t)ins_e::SYMBOL);
        auto* x = reinterpret_cast<atom_symbol_c*>(atom.get());
        auto encoded_len = pack<uint32_t>(x->data.size());
        ENCODE(data.bytes, encoded_len)
        auto encoded_data = pack_string(x->data);
        ENCODE(data.bytes, encoded_data);
        return;
      }
      case atom_type_e::INTEGER: {
        data.bytes.push_back((uint8_t)ins_e::INTEGER);
        auto* x = reinterpret_cast<atom_int_c*>(atom.get());
        auto encoded_len = pack<uint32_t>(sizeof(int64_t));
        ENCODE(data.bytes, encoded_len)
        auto encoded_data = pack<int64_t>(x->data);
        ENCODE(data.bytes, encoded_data);
        return;
      }
      case atom_type_e::REAL: {
        data.bytes.push_back((uint8_t)ins_e::INTEGER);
        auto* x = reinterpret_cast<atom_real_c*>(atom.get());
        auto encoded_len = pack<uint32_t>(sizeof(double));
        ENCODE(data.bytes, encoded_len)
        auto encoded_data = pack<double>(x->data);
        ENCODE(data.bytes, encoded_data);
        return;
      }
      case atom_type_e::STRING: {
        data.bytes.push_back((uint8_t)ins_e::STRING);
        auto* x = reinterpret_cast<atom_string_c*>(atom.get());
        auto encoded_len = pack<uint32_t>(x->data.size());
        ENCODE(data.bytes, encoded_len)
        auto encoded_data = pack_string(x->data);
        ENCODE(data.bytes, encoded_data);
        return;
      }
      case atom_type_e::LIST: {
        data_s inner_data{{}, data.err_cb};
        data.bytes.push_back((uint8_t)ins_e::LIST);
        auto* x = reinterpret_cast<atom_list_c*>(atom.get());
        for(size_t i = 0; i < x->data.size(); i++) {
          encode_atom(x->data[i], inner_data);
        }
        auto encoded_len = pack<uint32_t>(x->data.size());
        ENCODE(data.bytes, encoded_len)
        ENCODE(data.bytes, inner_data.bytes)
        return;
      }
    }
}

bytes_t generate_instructions(
    atom_list_t& list,
    generator_error_cb err_cb) {
  data_s data{{}, err_cb};
  for(size_t i = 0; i < list.size(); i++) {
    encode_atom(list[i], data);
  }
  return data.bytes;
}
