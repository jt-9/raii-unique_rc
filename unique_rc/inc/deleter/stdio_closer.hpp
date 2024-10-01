// memory_delete for unique_rc
#ifndef STDIO_DELETER_HPP
#define STDIO_DELETER_HPP

#include "defs.hpp"

#include <concepts>
#include <stdio.h>


RAII_NS_BEGIN

// Calls fclose() which flushes the stream pointed to by h and closes the underlying file descriptor.
struct stdio_fclose
{
  constexpr stdio_fclose() noexcept = default;

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  raii_inline void operator()(FILE *h) const noexcept { fclose(h); }
};

RAII_NS_END

#endif// STDIO_DELETER_HPP
