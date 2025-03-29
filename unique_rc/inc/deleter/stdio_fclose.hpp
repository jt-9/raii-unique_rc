#ifndef STDIO_FCLOSE_HPP
#define STDIO_FCLOSE_HPP

#include "raii_defs.hpp"

#include <stdio.h>


RAII_NS_BEGIN

// Calls fclose() which flushes the stream pointed to by h and closes the underlying file descriptor.
struct stdio_fclose
{
  constexpr stdio_fclose() noexcept = default;

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }
  [[nodiscard]] raii_inline static constexpr bool is_owned(FILE *h) noexcept { return h; }

  raii_inline void operator()(FILE *h) const noexcept { fclose(h); }
};

RAII_NS_END

#endif// STDIO_FCLOSE_HPP
