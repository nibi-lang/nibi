#pragma once

#include <libnibi/nibi.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern int32_t nibi_net_inet_addr(char *address);

API_EXPORT
extern int64_t nibi_net_errno();

API_EXPORT
extern void *nibi_net_sockaddr_in(int64_t family, int64_t port, int64_t addr);

API_EXPORT
extern int64_t nibi_net_setsocketopt(int64_t fd, int64_t level,
                                     int64_t opt_name, int64_t opt);

API_EXPORT
extern int64_t nibi_net_bind(int64_t fd, void *sockaddr_in);

API_EXPORT
extern int64_t nibi_net_connect(int64_t fd, void *sockaddr_in);

API_EXPORT
extern int64_t nibi_net_accept(int64_t fd, void *sockaddr_in);

API_EXPORT
extern int64_t nibi_net_read(int64_t fd, char *buffer, int64_t buffer_len);
}
