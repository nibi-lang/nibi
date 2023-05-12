#pragma once

#include <libnibi/cell.hpp>
#include <libnibi/environment.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern nibi::cell_ptr type(nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_numeric(nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_int(nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_double(nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_string(nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_list(nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr is_nil(nibi::cell_list_t &list, nibi::env_c &env);
}