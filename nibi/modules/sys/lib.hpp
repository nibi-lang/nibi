#pragma once

#include <libnibi/nibi.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern nibi::cell_ptr get_argv(nibi::cell_processor_if &ci,
                               nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr get_stdin(nibi::cell_processor_if &ci,
                                nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern nibi::cell_ptr get_platform(nibi::cell_processor_if &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr get_nibi_path(nibi::cell_processor_if &ci,
                                    nibi::cell_list_t &list, nibi::env_c &env);
}
