#pragma once

#include <libnibi/nibi.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern nibi::cell_ptr get_str(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                              nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr get_int(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                              nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr get_double(nibi::interpreter_c &ci,
                                 nibi::cell_list_t &list, nibi::env_c &env);
}