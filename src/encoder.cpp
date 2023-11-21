#include "encoder.hpp"

#include <fmt/format.h>
#include <functional>
#include <map>

namespace {

struct data_s {
  bytes_t bytes;
  generator_error_cb err_cb;
};

using embedded_builder = std::function<void(bytes_t&, generator_error_cb)>;

void generate_builtin_macro(bytes_t& data, generator_error_cb cb) {
  fmt::print("asked to generate a 'macro'\n");
}

void generate_builtin_if(bytes_t& data, generator_error_cb cb) {
  fmt::print("asked to generate 'if'\n");
}

static std::map<std::string, embedded_builder> embedded_builders = {
  { "macro", generate_builtin_macro },
  { "if", generate_builtin_if }
};

} // namespace

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

#define ENCODE(to_, from_) \
  to_.insert(to_.end(), from_.begin(), from_.end());

#define PARSE_LIST_AND_SCAN_BUILTINS(target_, list_, err_) \
  for(size_t i = 0; i < list_.size(); i++) { \
    data_s inner_data{{}, err_}; \
    auto specific_generator = encode_atom(list_[i], inner_data); \
    if (specific_generator.has_value()) { \
      (*specific_generator)(inner_data.bytes, err_); \
    } else { \
      inner_data.bytes.push_back((uint8_t)ins_e::EXECUTE_LIST);\
    }\
    ENCODE(target_, inner_data.bytes) \
  }

std::optional<embedded_builder> encode_atom(atom_ptr& atom, data_s &data) {

    switch(atom->type) {
      case atom_type_e::SYMBOL: {
        data.bytes.push_back((uint8_t)ins_e::SYMBOL);
        auto* x = reinterpret_cast<atom_symbol_c*>(atom.get());
        auto encoded_len = pack<uint32_t>(x->data.size());
        ENCODE(data.bytes, encoded_len)
        auto encoded_data = pack_string(x->data);
        ENCODE(data.bytes, encoded_data);

        auto fn = embedded_builders.find(x->data);
        if (fn != embedded_builders.end()) {
          return {fn->second};
        }
        return {};
      }
      case atom_type_e::INTEGER: {
        data.bytes.push_back((uint8_t)ins_e::INTEGER);
        auto* x = reinterpret_cast<atom_int_c*>(atom.get());
        auto encoded_data = pack<int64_t>(x->data);
        ENCODE(data.bytes, encoded_data);
        return {};
      }
      case atom_type_e::REAL: {
        data.bytes.push_back((uint8_t)ins_e::INTEGER);
        auto* x = reinterpret_cast<atom_real_c*>(atom.get());
        auto encoded_data = pack<double>(x->data);
        ENCODE(data.bytes, encoded_data);
        return {};
      }
      case atom_type_e::STRING: {
        data.bytes.push_back((uint8_t)ins_e::STRING);
        auto* x = reinterpret_cast<atom_string_c*>(atom.get());
        auto encoded_len = pack<uint32_t>(x->data.size());
        ENCODE(data.bytes, encoded_len)
        auto encoded_data = pack_string(x->data);
        ENCODE(data.bytes, encoded_data);
        return {};
      }
      case atom_type_e::LIST: {
        data.bytes.push_back((uint8_t)ins_e::LIST);
        auto* x = reinterpret_cast<atom_list_c*>(atom.get());

        bytes_t program;
        PARSE_LIST_AND_SCAN_BUILTINS(program, x->data, data.err_cb)

        auto encoded_len = pack<uint32_t>(program.size());
        ENCODE(data.bytes, encoded_len)
        ENCODE(data.bytes, program)
        return {};
      }
    }
}

bytes_t generate_instructions(
    atom_list_t& list,
    generator_error_cb err_cb) {

  bytes_t program;
  PARSE_LIST_AND_SCAN_BUILTINS(program, list, err_cb)
  return program;
}
