#include "bytecode_generator.hpp"
#include "builtins.hpp"
#include <fmt/format.h>
#include "nibi.hpp"

/*
      Regular list '()' with no builtins

          LIST <list len:bytes> 
            SYMBOL<len:bytes> | LIST <len:bytes>
                              .
                              .
            SYMBOL<len:bytes> | LIST <len:bytes>
            EXECUTE     < Use first item loaded as expected function to execute >


      Builtin bytes
          LIST <len:bytes>
            ... variable ... 

*/

static builtin_map* embedded_builders{nullptr};

#define ENCODE(to_, from_) \
  to_.insert(to_.end(), from_.begin(), from_.end());

#define PARSE_LIST_AND_SCAN_BUILTINS(target_, list_) \
  for(size_t i = 0; i < list_.size(); i++) { \
    bytes_t inner_data;\
    auto specific_generator = encode_atom(list_[i], inner_data); \
    if (specific_generator.has_value()) { \
      (*specific_generator)(inner_data, list_); \
    } else { \
      inner_data.push_back((uint8_t)ins_e::EXECUTE_LIST);\
    }\
    ENCODE(target_, inner_data) \
  }

std::optional<embedded_builder> encode_atom(atom_ptr& atom, bytes_t& data) {

    switch(atom->type) {
      case atom_type_e::SYMBOL: {
        data.push_back((uint8_t)ins_e::SYMBOL);
        auto* x = reinterpret_cast<atom_symbol_c*>(atom.get());
        auto encoded_len = pack<uint32_t>(x->data.size());
        ENCODE(data, encoded_len)
        auto encoded_data = pack_string(x->data);
        ENCODE(data, encoded_data);

        auto fn = embedded_builders->find(x->data);
        if (fn != embedded_builders->end()) {
          return {fn->second};
        }
        return {};
      }
      case atom_type_e::INTEGER: {
        data.push_back((uint8_t)ins_e::INTEGER);
        auto* x = reinterpret_cast<atom_int_c*>(atom.get());
        auto encoded_data = pack<int64_t>(x->data);
        ENCODE(data, encoded_data);
        return {};
      }
      case atom_type_e::REAL: {
        data.push_back((uint8_t)ins_e::INTEGER);
        auto* x = reinterpret_cast<atom_real_c*>(atom.get());
        auto encoded_data = pack<double>(x->data);
        ENCODE(data, encoded_data);
        return {};
      }
      case atom_type_e::STRING: {
        data.push_back((uint8_t)ins_e::STRING);
        auto* x = reinterpret_cast<atom_string_c*>(atom.get());
        auto encoded_len = pack<uint32_t>(x->data.size());
        ENCODE(data, encoded_len)
        auto encoded_data = pack_string(x->data);
        ENCODE(data, encoded_data);
        return {};
      }
      case atom_type_e::LIST: {
        data.push_back((uint8_t)ins_e::LIST);
        auto* x = reinterpret_cast<atom_list_c*>(atom.get());

        bytes_t program;
        PARSE_LIST_AND_SCAN_BUILTINS(program, x->data)

        auto encoded_len = pack<uint32_t>(program.size());
        ENCODE(data, encoded_len)
        ENCODE(data, program)
        return {};
      }
    }
  
  g_nibi->shutdown(2,
    fmt::format(
      "Internal Error: Unhandled atom type in {}\n", __FILE__));
}

bytes_t generate_instructions(atom_list_t& list) {

  if (!embedded_builders) {
    embedded_builders = &get_builders();
  }

  bytes_t program;
  PARSE_LIST_AND_SCAN_BUILTINS(program, list)
  return program;
}
