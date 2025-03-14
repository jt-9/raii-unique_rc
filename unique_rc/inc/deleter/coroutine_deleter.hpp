#ifndef COROUTINE_DELETER_HPP
#define COROUTINE_DELETER_HPP

#include "raii_defs.hpp"

#include <coroutine>


RAII_NS_BEGIN

template<typename Promise> struct coroutine_deleter
{
  constexpr coroutine_deleter() noexcept = default;

  // template<typename P>
  //   requires std::is_convertible_v<P, Promise>
  // // cppcheck-suppress noExplicitConstructor intended converting constructor
  // raii_inline constexpr coroutine_deleter(const coroutine_deleter<std::coroutine_handle<P>> &) noexcept
  // {}

  [[nodiscard]] raii_inline static constexpr std::coroutine_handle<Promise> noexcept { return {}; }
  [[nodiscard]] raii_inline static constexpr bool is_valid(std::coroutine_handle<Promise> h) noexcept { return h; }

  template<typename P> raii_inline void operator()(std::coroutine_handle<P> h) const noexcept { h.destroy(); }
};

RAII_NS_END

#endif// COROUTINE_DELETER_HPP
