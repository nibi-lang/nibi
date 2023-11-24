#pragma once

#include "expected_forms.hpp"

#include <map>
#include <string>

namespace builtin_forms {

using form_map_t = std::map<std::string, form_list_c>;

static form_map_t get_forms() {

  form_map_t fmap;

  // ---------- 'fn' ---------- 
  {
    auto x = form_list_c(5);
    x.expected_list.emplace_back(
      std::make_unique<form_symbol_c>("fn"));

    // Name of the fn
    x.expected_list.emplace_back(
      std::make_unique<form_symbol_c>());

    // Parameter declarations
    x.expected_list.emplace_back(
      std::make_unique<expected_form_c>(
        form_type_e::PARAM_DECL));

    // Some number of lists as the body
    x.expected_list.emplace_back(
      std::make_unique<form_body_c>());
  }
  // --------------------------
  
  // ---------- 'let' --------- 
  {
    auto x = form_list_c(4);
    x.expected_list.emplace_back(
      std::make_unique<form_symbol_c>("let"));

    // Target assignment name
    x.expected_list.emplace_back(
      std::make_unique<form_symbol_c>());

    // Target
    x.expected_list.emplace_back(
      std::make_unique<expected_form_c>(
        form_type_e::ANY));
  }
  // --------------------------

  return fmap;
}





} // namespace
