#pragma once

#define KIBI_BYTE 1024
#define MEBI_BYTE KIBI_BYTE * KIBI_BYTE

// Preallocation size of program instruction
// size in memory
#ifndef FILE_EXEC_PREALLOC_SIZE
#define FILE_EXEC_PREALLOC_SIZE MEBI_BYTE * 2
#endif


// Simple debugging macros
#ifdef NIBI_DEBUG
#include <fmt/format.h>

#define DEBUG_OUT(message_) \
  fmt::print(stderr, "{}:{} > {}\n", __FILE__, __LINE__, message_);

#else

#define DEBUG_OUT(message_)

#endif
