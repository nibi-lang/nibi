#include "builtins.hpp"

void generate_builtin_macro(bytes_t& data, atom_list_t& list) {
//  fmt::print("asked to generate a 'macro'\n");
}

void generate_builtin_let(bytes_t& data, atom_list_t& list) {
 // fmt::print("asked to generate 'let'\n");
 // print_atom_list(list);
 // fmt::print("\n\n");


  // Swap out <op,len,data> for LET symbol load
  // with ASSIGN

  // Ensure that the first param is a variable name

  // Add instruction to ensure it doesn't exist yet

  // complete

  // May be beneficial to extern the encode_atom method
  // so we can re-encode here if we want to... though that
  // would be wasteful.....
}

void generate_builtin_if(bytes_t& data, atom_list_t& list) {
//  fmt::print("asked to generate 'if'\n");
}
static std::map<std::string, embedded_builder> embedded_builders = {
  { "macro", generate_builtin_macro },
  { "if", generate_builtin_if },
  { "let", generate_builtin_let },
};


builtin_map& get_builders() {
  return embedded_builders;
}
