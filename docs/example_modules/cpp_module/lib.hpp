#pragma once

#include <libnibi/cell.hpp>
#include <libnibi/environment.hpp>

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

/*
  NOTE: Any function that is to be exported to nibi must be declared within
        the dylib list in order to be brought into the environment.

        For private members, start the function off with an underscore

*/

API_EXPORT
extern nibi::cell_ptr example_function(nibi::cell_list_t &list, nibi::env_c &env);

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