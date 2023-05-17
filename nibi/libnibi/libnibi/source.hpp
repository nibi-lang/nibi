#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

namespace nibi {
//! \brief A locator interface.
//!       Used to locate errors in the source.
class locator_if {
public:
  virtual ~locator_if() = default;
  virtual const size_t get_line() const = 0;
  virtual const size_t get_column() const = 0;
  virtual const char *get_source_name() const = 0;
  virtual std::tuple<size_t, size_t> get_line_column() const = 0;
};

// Shorthand for a shared locator interface pointer.
using locator_ptr = std::shared_ptr<locator_if>;

extern void draw_locator(locator_if &location);

//! \brief A source origin. (File, string, etc.)
//!        Used to create locators.
class source_origin_c {
public:
  source_origin_c() = delete;
  source_origin_c(const source_origin_c &) = delete;
  source_origin_c(source_origin_c &&) = delete;

  //! \brief Create a source origin.
  //! \param source_name The name of the source.
  source_origin_c(const std::string source_name) : source_name_(source_name) {}
  //! \brief Get the name of the source.
  const std::string get_source_name() const { return source_name_; }
  //! \brief Get a locator interface for the source.
  locator_ptr get_locator(const size_t line, const size_t column) const {
    return std::make_shared<locator_c>(source_name_.c_str(), line, column);
  }

private:
  class locator_c final : public locator_if {
  public:
    locator_c(const char *source, const size_t line, const size_t column)
        : line_(line), column_(column), source_name_(source) {}
    virtual std::tuple<size_t, size_t> get_line_column() const override {
      return std::make_tuple(line_, column_);
    }
    virtual const size_t get_line() const override { return line_; }
    virtual const size_t get_column() const override { return column_; }
    virtual const char *get_source_name() const override {
      return source_name_;
    }

  private:
    const size_t line_{0};
    const size_t column_{0};
    const char *source_name_{nullptr};
  };
  const std::string source_name_;
};

//! \brief A source manager that manages all the sources.
class source_manager_c {
public:
  //! \brief Get a source by name.
  //! \param source_name The name of the source.
  //! \note If the source does not exist, it will be created.
  std::shared_ptr<source_origin_c> get_source(const std::string &source_name) {
    auto it = sources_.find(source_name);
    if (it != sources_.end()) {
      return it->second;
    }
    auto source = std::make_shared<source_origin_c>(source_name);
    sources_.insert(std::make_pair(source_name, source));
    return source;
  }
  //! \brief Check if a source exists.
  bool exists(const std::string &source_name) const {
    return sources_.find(source_name) != sources_.end();
  }
  inline void clear() { sources_.clear(); }

private:
  std::unordered_map<std::string, std::shared_ptr<source_origin_c>> sources_;
};
} // namespace nibi