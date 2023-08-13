#pragma once

#include <cstdint>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern int32_t nibi_io_printf(const char * format, char *data);

}
