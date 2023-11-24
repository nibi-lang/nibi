#pragma once

#include <string>
#include <memory>
#include <vector>

enum class form_type_e {
  SYMBOL,
  BODY,
  PARAM_DECL,
  LIST,
  ANY,  // int, real, list, or sym
};

class expected_form_c {
public:
  expected_form_c() = delete;
  expected_form_c(const form_type_e& type)
    : type(type){}
  form_type_e type;
};

using expected_form_ptr = std::unique_ptr<expected_form_c>;
using expected_list_t = std::vector<expected_form_ptr>;

class form_symbol_c : public expected_form_c {
public:
  form_symbol_c() : expected_form_c(form_type_e::SYMBOL){}
  form_symbol_c(const std::string& sym)
    : expected_form_c(form_type_e::SYMBOL) {
      specific_sym = {sym};
    }
  std::optional<std::string> specific_sym{std::nullopt};
};

class form_body_c : public expected_form_c {
public:
  form_body_c() : expected_form_c(form_type_e::BODY){}
  form_body_c(const size_t& max_size)
    : expected_form_c(form_type_e::BODY) {
      this->max_size = {max_size};
    }
  std::optional<size_t> max_size;
};

class form_list_c : public expected_form_c {
public:
  form_list_c()
    : expected_form_c(form_type_e::LIST){}
  form_list_c(const size_t& min_len)
    : expected_form_c(form_type_e::LIST){
      this->min_length = {min_len};
    }
  form_list_c(expected_list_t ex_list)
    : expected_form_c(form_type_e::LIST),
      expected_list(std::move(ex_list)){}
  expected_list_t expected_list;
  std::optional<size_t> min_length{std::nullopt};
  std::optional<size_t> max_length{std::nullopt};
};

