#pragma once

#include <string>

namespace front {

struct pos_s {
  size_t line{0};
  size_t col{0};
  pos_s(size_t l, size_t c)
    : line(l), col(c){}
};

struct error_s {
  std::string message;
  pos_s pos;
};

} // namespace
