#pragma once

#include <libnibi/nibi.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern nibi::cell_ptr nibi_threads_future(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_threads_future_get(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_threads_future_wait_for(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_threads_future_is_ready(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_threads_future_runtime(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_threads_future_kill(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_module_create(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_module_destroy(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);

}
