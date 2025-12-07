#pragma once

#include <libnibi/nibi.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type-c-linkage"
#endif

extern "C" {

API_EXPORT
extern nibi::cell_ptr nibi_get_str(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_get_int(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_get_double(nibi::interpreter_c &ci,
                                      nibi::cell_list_t &list,
                                      nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_prompt_for(nibi::interpreter_c &ci,
                                      nibi::cell_list_t &list,
                                      nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_print(nibi::interpreter_c &ci,
                                 nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_println(nibi::interpreter_c &ci,
                                   nibi::cell_list_t &list, nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_flush(nibi::interpreter_c &ci,
                                 nibi::cell_list_t &list, nibi::env_c &env);

API_EXPORT
extern void nibi_runtime_init();

API_EXPORT
extern void nibi_runtime_deinit();
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
