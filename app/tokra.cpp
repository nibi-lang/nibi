#include <iostream>
#include <vector>
#include <string>

#include "compiler/error.hpp"
#include "compiler/source.hpp"
#include "compiler/input.hpp"
#include "compiler/list.hpp"

// stub class to receive the list - this will be the type checker later
class list_receiver_stub final : public list_cb_if {
public:
  void on_list(std::unique_ptr<list_c> list) override {}
};

int main(int argc, char** argv) {

  std::vector<std::string> args(argv, argv + argc);
  for(auto arg : args) {
    std::cout << arg << std::endl;
  }

  source_manager_c source_manager;

  list_receiver_stub stub;

  list_builder_c builder(stub);

  file_reader_c reader(builder, source_manager);
  reader.read_file("input.tokra");

  return 0;
}