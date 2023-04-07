#include <iostream>
#include <vector>
#include <string>

#include "compiler/error.hpp"
#include "compiler/source.hpp"
#include "compiler/input.hpp"

class stub_cb final : public scanner_cb_if {
public:
  void on_token(token_c token) override {
    auto enum_value = token.get_token();
    auto data = token.get_data();
    if (data.size()) {
      std::cout << "token with data: " << (int)enum_value << " " << data << std::endl;
    } else {
      std::cout << "token without data: " << (int)enum_value << std::endl;
    }
  }
  void on_error(error_c error) override {
    error.draw_error();
  }
  void on_complete(std::optional<unclosed_type_e> unclosed_symbol) override {
    if (unclosed_symbol.has_value()) {
      std::cout << "unclosed symbol: " << (int)unclosed_symbol.value() << std::endl;
    } else {
      std::cout << "no unclosed symbols" << std::endl;
    }
  }
};

int main(int argc, char** argv) {
  std::cout << "hello world\n";

  std::vector<std::string> args(argv, argv + argc);

  for(auto arg : args) {
    std::cout << arg << std::endl;
  }


  source_manager_c source_manager;
  auto source = source_manager.get_source("test.tokra");
  auto locator = source->get_locator(1, 90);
  std::cout << locator->get_source_name() << std::endl;
  std::cout << locator->get_line() << std::endl;
  std::cout << locator->get_column() << std::endl;

  auto err = error_c(std::move(locator), "test error");

  err.draw_error();

  stub_cb cb;
  file_reader_c reader(cb, source_manager);
  reader.read_file("input.tokra");

  return 0;
}