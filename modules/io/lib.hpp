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
extern cell_ptr get_str(cell_list_t &list, env_c &env);

}