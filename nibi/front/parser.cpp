#include "parser.hpp"

#include <iostream>

namespace front {

void parser_c::on_error(error_s error) {

  std::cout << "Parser received an error report" << std::endl;
}

void parser_c::on_top_list_complete() {

  std::cout << "Parser needs to run a queue" << std::endl;
}

void parser_c::on_list(atom_list_t list) {

  std::cout << "Parser got an atom list : " << list.size() << std::endl;
  print_list(list);
}

} // namespace
