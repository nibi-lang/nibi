#pragma once

#include <libnibi/nibi.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern nibi::cell_ptr print(nibi::cell_processor_if &ci,
                            nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr println(nibi::cell_processor_if &ci,
                              nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr get_str(nibi::cell_processor_if &ci,
                              nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr get_int(nibi::cell_processor_if &ci,
                              nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr get_double(nibi::cell_processor_if &ci,
                                 nibi::cell_list_t &list, nibi::env_c &env);
}
