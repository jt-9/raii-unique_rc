#ifndef STDIO_FCLOSE_HPP
#define STDIO_FCLOSE_HPP

#include "raii_defs.hpp"

#include <stdio.h>


RAII_NS_BEGIN

// Calls fclose() which flushes the stream pointed to by h and closes the underlying file descriptor.
struct stdio_fclose
{
  constexpr stdio_fclose() noexcept = default;

#ifdef __cpp_static_call_operator
  // False poisitive, guarded by feature #ifdef __cpp_static_call_operator
  // NOLINTNEXTLINE(clang-diagnostic-c++23-extensions)
  raii_inline static void operator()(FILE *h) noexcept
#else
  raii_inline void operator()(FILE *h) const noexcept
#endif
  {
    fclose(h);
  }
};

RAII_NS_END

#endif// STDIO_FCLOSE_HPP
