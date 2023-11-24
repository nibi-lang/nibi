//  Structures that can be overlayed onto
//  an encoded atom list to decode the data,
//  along with some helper functions

#pragma once

#include "atoms.hpp"

namespace atom_view {

#pragma pack(push, 1)

struct len_encoded_c {
  uint16_t len;
  uint8_t data[];
};

struct int_encoded_c {
  int64_t value;
};

struct real_encoded_c {
  double value;
};

struct view_s {
  uint8_t id;
  uint32_t line;
  uint32_t col;
  union {
    len_encoded_c len_encoded;
    int_encoded_c int_encoded;
    real_encoded_c real_encoded;
  } data;
};

#pragma pack(pop)

static constexpr uint8_t HEADER_SIZE = sizeof(uint8_t) +
                                       sizeof(uint32_t) +
                                       sizeof(uint32_t);

static inline std::size_t get_size(const view_s* view, bool& valid) {
  valid = true;
  switch (view->id) {
    case (int)atom_type_e::SYMBOL:
    case (int)atom_type_e::STRING:
    case (int)atom_type_e::LIST: {
      return HEADER_SIZE + 
             sizeof(uint16_t) +
             view->data.len_encoded.len;
    }
    case (int)atom_type_e::REAL:
      return HEADER_SIZE + 
              sizeof(double);
    case (int)atom_type_e::INTEGER:
      return HEADER_SIZE + 
              sizeof(int64_t);
  };
  valid = false;
  return 0;
}

static inline std::string view_to_string(const view_s* view, bool quote_str=true) {
  switch (view->id) {
    case (int)atom_type_e::SYMBOL:
      return std::string(
        (char*)(view->data.len_encoded.data), view->data.len_encoded.len);
    case (int)atom_type_e::STRING: {
      if (!quote_str) {
        return std::string(
          (char*)(view->data.len_encoded.data), view->data.len_encoded.len);
      }
      return fmt::format("\"{}\"", 
          std::string(
            (char*)(view->data.len_encoded.data), 
            view->data.len_encoded.len));
    }
    case (int)atom_type_e::REAL:
      return std::to_string(view->data.real_encoded.value);
    case (int)atom_type_e::INTEGER:
      return std::to_string(view->data.int_encoded.value);
    case (int)atom_type_e::LIST: {
      std::string result = "( ";
      std::size_t pc{0};
      bool valid{true};
      while(valid && pc < view->data.len_encoded.len) {
        atom_view::view_s* v =
          (atom_view::view_s*)
          ((uint8_t*)(view->data.len_encoded.data) + pc);
        result += view_to_string(v, quote_str);
        result += " ";
        pc += get_size(v, valid);
      }
      result += ")";
      return result;
    }
  };
  return "";
}

} // namespace
