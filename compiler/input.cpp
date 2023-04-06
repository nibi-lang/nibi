#include "input.hpp"
#include <filesystem>
#include <fstream>

std::optional<error_c> file_reader_c::read_file(std::string_view path) {
  // Read the file and pass the contents to the scanner.
  std::filesystem::path file_path(path);

  if (!std::filesystem::exists(file_path)) {
    return error_c("File does not exist");
  }

  scanner_.reset();

  std::ifstream ifs; 
  ifs.open(file_path, std::ios::in);
  if (ifs.fail()) {
    return error_c("Failed to open file");
  }

  auto source_origin = sm_.get_source(file_path.string());

  std::string line;
  while(!ifs.eof()) {
    std::getline(ifs, line);
    scanner_.scan_line(source_origin, line);
  }

  ifs.close();

  scanner_.indicate_complete();

  return {};
}

void scanner_c::reset() {
  tracker_ = tracker_s{};
}

void scanner_c::indicate_complete() {
  // Check to ensure that the scanner is in a valid state,
  // such that all parens, brackets, and braces are closed.
}

bool scanner_c::scan_line(std::shared_ptr<source_origin_c> origin, std::string_view data) {
  tracker_.line_count++;
  // Scan the line and call the callback interface.

  // use origin to create a locator for each token found.

  // call cb_.on_token(token_c(locator, <TOKEN>));
  return true;
}