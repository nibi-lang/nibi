#pragma once

#include "input.hpp"
#include "token.hpp"
#include "source.hpp"

#include <memory>
#include <optional>
#include <vector>

enum class base_types_e {
  NIL,
  INTEGER,
  FLOAT,
  STRING,
  LIST,
  FN,
  PROMISE
};

enum class implied_types_e {
  NIL,
  NUMERICAL,
  STRING,
  LIST,
  FN,
  PROMISE
};

enum class element_type_e {
  NIL,
  INTEGER,
  FLOAT,
  STRING,
  SYMBOL,
  LIST,
  FN,
  PROMISE
};

class element_base_c {
public:
  element_base_c() = delete;
  element_base_c(element_type_e type) : type(type) {}
  element_base_c(element_type_e type, const std::string data) : type(type), data(data) {}
  element_base_c(element_type_e type, const std::string data, locator_if& locator)
    : type(type), 
      data(data),
      locator(&locator) {}
  virtual ~element_base_c() = default;
  element_type_e type{element_type_e::NIL};
  std::string data{""};

  // The locator interface for this element iff a locator is available
  locator_if* locator{nullptr};

  // Type explicitly declared by the user
  std::optional<base_types_e> declared_type{std::nullopt};

  // Type implied by the context
  std::optional<implied_types_e> implied_type{std::nullopt};
};

class list_c final : element_base_c {
public:
  list_c() : element_base_c(element_type_e::LIST) {}
  std::vector<std::unique_ptr<element_base_c>> elements;
};

class fn_c final : element_base_c {
public:
  fn_c() : element_base_c(element_type_e::FN) {}
  // fn name encoded in element_base_c::data
  std::vector<std::unique_ptr<element_base_c>> parameters;
};

//! \brief A callback interface for the list builder
class list_cb_if {
public:
  virtual ~list_cb_if() = default;
  //! \brief Called when a list is found from tokens
  virtual void on_list(std::unique_ptr<list_c> list) = 0;
};

//! \brief A list builder that constructs lists as tokens are
//!        scanned and sent to it. Once a full list is detected
//!        it will be sent to the callback to be analyzed
class list_builder_c final : public scanner_cb_if {
public:
  list_builder_c() = delete;
  list_builder_c(list_cb_if& cb);
  void on_token(token_c token, bool end_list=false) override;
  void on_error(error_c error) override;
  void on_complete(std::optional<unclosed_type_e> unclosed_symbol) override;
private:
  list_cb_if &cb_;
  std::vector<token_c> tokens_;
  std::vector<error_c> errors_;
};