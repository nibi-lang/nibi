#pragma once

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern int math_add(int a, int b);

API_EXPORT
extern int math_div(int a, int b);

}
