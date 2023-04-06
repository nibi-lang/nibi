#include <iostream>
#include <vector>
#include <string>

#include "compiler/error.hpp"
#include "compiler/source.hpp"

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

  return 0;
}