#pragma once

#include <libnibi/nibi.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern nibi::cell_ptr type(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                           nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_numeric(nibi::interpreter_c &ci,
                                 nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_int(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                             nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_double(nibi::interpreter_c &ci,
                                nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_string(nibi::interpreter_c &ci,
                                nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_list(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                              nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_nil(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                             nibi::env_c &env);
}
