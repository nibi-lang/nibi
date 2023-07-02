#pragma once

#include <libnibi/nibi.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern nibi::cell_ptr _dylib_create_rll(nibi::cell_processor_if &ci,
                               nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr _dylib_bind_fn(nibi::cell_processor_if &ci,
                               nibi::cell_list_t &list, nibi::env_c &env);
}
