#include <fmt/format.h>

#include "atomiser.hpp"

int main(int argc, char** argv) {

  std::optional<parse_group_s>file_group = atomise_file("test.nib");

  if (!file_group.has_value()) {
    fmt::print("No parse group returned\n");
    return 1;
  }

  fmt::print(
    "File: '{}' returned '{}' lists\n",
    (*file_group).origin,
    (*file_group).lists.size());

  return 0;
}
