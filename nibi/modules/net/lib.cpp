#include "lib.hpp"

#include <iostream>
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
  auto read_in = read(fd, buffer, buffer_len);
  if (read_in < buffer_len) {
    buffer[read_in] = '\0';
  }
  return read_in;
}
