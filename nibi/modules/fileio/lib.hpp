#pragma once

#include <libnibi/nibi.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern nibi::cell_ptr nibi_fopen(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_fclose(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_fputs(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_fprintf(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_fwrite(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_fgets(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_fgetc(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_feof(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_ftell(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_rewind(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_clearerr(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_ferror(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);
API_EXPORT
extern nibi::cell_ptr nibi_fseek(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env);

}
