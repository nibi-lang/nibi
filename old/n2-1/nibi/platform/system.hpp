#if defined(__linux__) || defined(__APPLE__) || defined(__unix__) || defined(__CYGWIN__)
  #define NIBI_PLATFORM_NIX 1
  #define NIBI_PLATFORM_WIN 0
  #include <unistd.h>
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  #define NIBI_PLATFORM_NIX 0
  #define NIBI_PLATFORM_WIN 1
  #include <io.h>
#else
  #error "Unknown platform detected"
#endif

#include <stdio.h>

namespace platform {

  inline bool stdin_is_tty() {
#if(NIBI_PLATFORM_NIX)
  return isatty(fileno(stdin));
#elif(NIBI_PLATFORM_WIN)
  return _isatty(_fileno(stdin));
#else
#warning "stdin_is_tty can't be trusted on current (unknown) platform"
    return false;
#endif
  }

  inline bool stderr_is_tty() {
#if(NIBI_PLATFORM_NIX)
  return isatty(fileno(stderr));
#elif(NIBI_PLATFORM_WIN)
  return _isatty(_fileno(stderr));
#else
#warning "stderr_is_tty can't be trusted on current (unknown) platform"
    return false;
#endif
  }

  inline bool stdout_is_tty() {
#if(NIBI_PLATFORM_NIX)
  return isatty(fileno(stdout));
#elif(NIBI_PLATFORM_WIN)
  return _isatty(_fileno(stdout));
#else
#warning "stdout_is_tty can't be trusted on current (unknown) platform"
    return false;
#endif
  }


} // namespace
