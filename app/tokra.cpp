#include <iostream>
#include <vector>
#include <string>

#include <runtime/cell.hpp>
#include <runtime/memory.hpp>
#include "compiler/error.hpp"
#include "compiler/source.hpp"
#include "compiler/input.hpp"
#include "compiler/list.hpp"

// stub class to receive the list - this will be the type checker later
class list_receiver_stub final : public list_cb_if {
public:
  void on_list(cell_c* list_cell) override
  {
    auto& list = list_cell->as_list();
    for(auto& cell : list) {
      std::cout << "Cell: " << cell->to_string() << std::endl;
    }
  }
};

int main(int argc, char** argv) {

  std::vector<std::string> args(argv, argv + argc);
  for(auto arg : args) {
    std::cout << arg << std::endl;
  }

  if (args.size() == 1) {
    std::cout << "No input file specified" << std::endl;
    return 1;
  }

  auto entry_file = args[1];

  source_manager_c source_manager;

  memory::controller_c<cell_c> instruction_memory;

  list_receiver_stub stub;


  // TODO: Pass the instruction memory to the list builder, 
  //       so that it can create cells and add them to the instruction memory
  //       - Also need to change list_receiver to take in a cell_c* 

  list_builder_c builder(instruction_memory, stub);

  file_reader_c reader(builder, source_manager);
  reader.read_file(entry_file);

  return 0;
}