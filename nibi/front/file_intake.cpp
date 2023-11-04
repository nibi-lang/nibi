#include "file_intake.hpp"
#include "lexer.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

namespace front {

extern bool from_file(
  front::atom_receiver_if &receiver,
  const std::string &file_name) {

  auto emit_error = [&](const std::string& msg) {

    std::string error_message = file_name;
    error_message += ": ";
    error_message += msg;

    error_s e{error_message, {0, 0}};
    receiver.on_error(e);
  };

  {
    std::filesystem::path path(file_name);
    if (!std::filesystem::is_regular_file(path)) {
      emit_error("Not a regular file");
      return false;
    }
  }

  std::ifstream in(file_name);
  if (!in.is_open()) {
    emit_error("Unable to open file");
    return false;
  }

  lexer_c lexer(receiver);

  size_t line_no{1};
  std::string line;
  while(std::getline(in, line)) {
    lexer.submit(line, line_no++);
  }

  in.close();

  return lexer.finish();
}

} // namespace
