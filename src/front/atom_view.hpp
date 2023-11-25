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

static inline bool is_symbol(view_s* v) {
  if (!v) return false;
  return v->id == (int)atom_type_e::SYMBOL;
}

static inline bool is_string(view_s* v) {
  if (!v) return false;
  return v->id == (int)atom_type_e::STRING;
}

static inline bool is_list(view_s* v) {
  if (!v) return false;
  return v->id == (int)atom_type_e::LIST;
}

static inline bool is_int(view_s* v) {
  if (!v) return false;
  return v->id == (int)atom_type_e::INTEGER;
}

static inline bool is_real(view_s* v) {
  if (!v) return false;
  return v->id == (int)atom_type_e::REAL;
}

static bool string_data(view_s* v, std::string& val) {
  if (!is_string(v) && !is_symbol(v)) return false;
  val = std::string(
    (char*)(v->data.len_encoded.data), 
            v->data.len_encoded.len);
  return true;
}

static bool int_data(view_s* v, int64_t& val) {
  if (!is_int(v)) return false;
  val = v->data.int_encoded.value;
  return true;
}

static bool real_data(view_s* v, double& val) {
  if (!is_int(v)) return false;
  val = v->data.real_encoded.value;
  return true;
}

class walker_c {
public:
  walker_c() = delete;
  walker_c(
    std::vector<uint8_t>& data)
    : _raw_data(data.data()),
      _size(data.size()){}

  walker_c(uint8_t* raw, std::size_t size)
    : _raw_data(raw),
      _size(size){}

  view_s* next() {
    if (is_complete()) {
      return nullptr;
    }
    atom_view::view_s* v =
      (atom_view::view_s*) (_raw_data + _counter);

    _counter += get_size(v, _valid);
    return v;
  };

  void skip() {
    next();
  }

  void mark() {
    _marker = _counter;
  }

  void go_to_mark() {
    _counter = _marker;
  }

  void reset() {
    _valid = true;
    _counter = 0;
  }

  bool is_valid() const { return _valid; }
  bool has_next() const { return _counter < _size; }
  bool is_complete() const { return !has_next(); }
private:
  bool _valid{true};
  std::size_t _counter{0};
  std::size_t _marker{0};

  uint8_t* _raw_data{nullptr};
  std::size_t _size{0};
};

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

      walker_c walker(
        (uint8_t*)view->data.len_encoded.data,
        (std::size_t)view->data.len_encoded.len);

      std::string result = "( ";
      while(walker.has_next()) {
        result += fmt::format(
          "{} ", 
          view_to_string(walker.next(), quote_str));
      }
      result += ")";
      return result;
    }
  };
  return "";
}

} // namespace
