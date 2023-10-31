#include <parser/parser.hpp>

#include <iostream>

void list_receiver(parser::list_t list) {
  std::cout << "Received list of length: " << list.size() << std::endl;
  parser::print_list(list);
}

void error_receiver(parser::error_s err) {
  std::cout << "Error (" << err.line << ":" << err.col << ") " << err.message << std::endl;
  std::exit(1);
}

int main(int argc, char **argv) {


  std::string test_line = "(+ (- - - - (* * *)  - - ) + + + )";

  parser::parser_c parser(list_receiver, error_receiver);

  parser.submit(test_line);

  parser.finish();

  return 0;
}
