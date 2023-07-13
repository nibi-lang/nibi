#pragma once

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern int return_true();

API_EXPORT
extern int echo_int(int i);
}
