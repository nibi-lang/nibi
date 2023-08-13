#pragma once
#include <cstring>

#define NIBI_COPY_STRING(from, to) \
{ \
  auto len = strlen(from); \
  to = new char[len+1];\
  strncpy(to, from, len); \
  to[len] = '\0';\
}

#define NIBI_OBJECT_CLONE_TRIVIAL(from, to) \
  to->type = from->type;\
  if (from->type == object_type_e::STRING) { \
    NIBI_COPY_STRING(from->data.s, to->data.s);\
  } else { \
    to->data = from->data;\
  }
