#include "lib.hpp"
#include <cstdio>
int32_t nibi_io_printf(const char* format, char *data) {
  return printf(format, data);
}
