#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include "libnibi/ref.hpp"
#include "config.hpp"

namespace nibi {
//! \brief A locator interface.
//!       Used to locate errors in the source.
class locator_if : public ref_counted_c {
public:
  virtual ~locator_if() = default;
  virtual const size_t get_line() const = 0;
  virtual const size_t get_column() const = 0;
  virtual const char *get_source_name() const = 0;
  virtual std::tuple<size_t, size_t> get_line_column() const = 0;
  virtual nibi::ref_counted_ptr_c<locator_if> clone() = 0;
};

// Shorthand for a shared locator interface pointer.
using locator_ptr = nibi::ref_counted_ptr_c<locator_if>;

extern void draw_locator(locator_if &location);

//! \brief A locator implementation.
class locator_c final : public locator_if {
public:
  //! \brief Create the locator.
  //! \param source The name of the source.
  //! \param line The line of the source.
  //! \param column The column of the source.
  locator_c(std::shared_ptr<std::string> source, const size_t line,
            const size_t column)
      : line_(line), column_(column), source_name_(source) {}
  virtual std::tuple<size_t, size_t> get_line_column() const override {
    return std::make_tuple(line_, column_);
  }
  virtual const size_t get_line() const override { return line_; }
  virtual const size_t get_column() const override { return column_; }
  virtual const char *get_source_name() const override {
    return source_name_->c_str();
  }
  virtual locator_ptr clone() override {
    return new locator_c(source_name_, line_, column_);
  }

private:
  const config::locator_line_underlying_type_t line_{0};
  const config::locator_column_underlying_type_t column_{0};
  std::shared_ptr<std::string> source_name_{nullptr};
};

//! \brief A source origin. (File, string, etc.)
//!        Used to create locators.
class source_origin_c {
public:
  source_origin_c() = delete;
  source_origin_c(const source_origin_c &) = delete;
  source_origin_c(source_origin_c &&) = delete;

  //! \brief Create a source origin.
  //! \param source_name The name of the source.
  source_origin_c(std::string source_name)
      : source_name_(std::make_shared<std::string>(source_name)) {}

  //! \brief Get the name of the source.
  std::string get_source_name() { return *source_name_; }
  //! \brief Get a locator interface for the source.
  locator_ptr get_locator(const size_t line, const size_t column) const {
    return new locator_c(source_name_, line, column);
  }

private:
  std::shared_ptr<std::string> source_name_{nullptr};
};

//! \brief A source manager that manages all the sources.
class source_manager_c {
public:
  //! \brief Get a source by name.
  //! \param source_name The name of the source.
  //! \note If the source does not exist, it will be created.
  std::shared_ptr<source_origin_c> get_source(const std::string source_name) {
    auto it = sources_.find(source_name);
    if (it != sources_.end()) {
      return it->second;
    }
    auto source = std::make_shared<source_origin_c>(source_name);
    sources_.insert(std::make_pair(source_name, source));
    return source;
  }
  //! \brief Check if a source exists.
  bool exists(const std::string source_name) const {
    return sources_.find(source_name) != sources_.end();
  }
  inline void clear() { sources_.clear(); }

private:
  std::unordered_map<std::string, std::shared_ptr<source_origin_c>> sources_;
};
} // namespace nibi
