#include "lib.hpp"

#include <iostream>
#include <map>
#include <string>

#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

int32_t nibi_net_inet_addr(char *address) {
  return (int32_t)inet_addr(address);
}

int64_t nibi_net_errno() { return (int64_t)errno; }

void *nibi_net_sockaddr_in(int64_t family, int64_t port, int64_t addr) {
  auto *address = new sockaddr_in();
  address->sin_family = family;
  address->sin_port = port;
  address->sin_addr.s_addr = addr;
  return (void *)address;
}

int64_t nibi_net_setsocketopt(int64_t fd, int64_t level, int64_t opt_name,
                              int64_t opt) {
  return setsockopt(fd, level, opt_name, &opt, sizeof(opt));
}

int64_t nibi_net_bind(int64_t fd, void *saddr) {
  return bind(fd, (struct sockaddr *)saddr, sizeof(struct sockaddr_in));
}

int64_t nibi_net_connect(int64_t fd, void *saddr) {
  return connect(fd, (struct sockaddr *)saddr, sizeof(struct sockaddr_in));
}

int64_t nibi_net_accept(int64_t fd, void *saddr) {
  auto addrlen = sizeof(struct sockaddr_in);
  return accept(fd, (struct sockaddr *)saddr, (socklen_t *)&addrlen);
}

int64_t nibi_net_read(int64_t fd, char *buffer, int64_t buffer_len) {
  return read(fd, buffer, buffer_len);
}

namespace {
std::map<std::string, int64_t> constants = {
    {"AF_UNIX", AF_UNIX},           {"AF_INET", AF_INET},
    {"SOCK_STREAM", SOCK_STREAM},   {"SOCK_DGRAM", SOCK_DGRAM},

    {"IPPROTO_TCP", IPPROTO_TCP},   {"IPPROTO_UDP", IPPROTO_UDP},

    {"SO_DEBUG", SO_DEBUG},         {"SO_BROADCAST", SO_BROADCAST},
    {"SO_REUSEADDR", SO_REUSEADDR}, {"SO_KEEPALIVE", SO_KEEPALIVE},
    {"SO_LINGER", SO_LINGER},       {"SO_REUSEPORT", SO_REUSEPORT},
    {"SO_OOBINLINE", SO_OOBINLINE}, {"SO_SNDBUF", SO_SNDBUF},
    {"SO_RCVBUF", SO_RCVBUF},       {"SO_DONTROUTE", SO_DONTROUTE},
    {"SO_RCVLOWAT", SO_RCVLOWAT},   {"SO_RCVTIMEO", SO_RCVTIMEO},
    {"SO_SNDLOWAT", SO_SNDLOWAT},   {"SO_SNDTIMEO", SO_SNDTIMEO},
    {"SOL_SOCKET", SOL_SOCKET},     {"INADDR_ANY", INADDR_ANY},
    {"SHUT_RD", SHUT_RD},           {"SHUT_WR", SHUT_WR},
    {"SHUT_RDWR", SHUT_RDWR}};
}

int64_t nibi_net_get_const(char *name) {
  std::string c = name;
  auto x = constants.find(name);
  if (x == constants.end()) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  }
  return constants[name];
}
