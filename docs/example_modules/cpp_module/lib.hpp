#pragma once

#include <libnibi/cell.hpp>
#include <libnibi/environment.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

/*
  NOTE: Any function that is to be exported to nibi must be declared within
        the dylib list in order to be brought into the environment.

        For private members, start the function off with an underscore

*/

API_EXPORT
extern nibi::cell_ptr example_function(nibi::cell_list_t &list, nibi::env_c &env);
}