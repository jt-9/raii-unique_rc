#ifndef COROUTINE_DESTROY_HPP
#define COROUTINE_DESTROY_HPP

#include "raii_defs.hpp"

#include <coroutine>


RAII_NS_BEGIN

template<typename Promise> struct coroutine_destroy
{
  constexpr coroutine_destroy() noexcept = default;

  // template<typename P>
  //   requires std::is_convertible_v<P, Promise>
  // // cppcheck-suppress noExplicitConstructor intended converting constructor
  // raii_inline constexpr coroutine_destroy(const coroutine_destroy<std::coroutine_handle<P>> &) noexcept
  // {}

  [[nodiscard]] raii_inline static constexpr std::coroutine_handle<Promise> invalid() noexcept { return {}; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(std::coroutine_handle<Promise> h) noexcept
  {
    return static_cast<bool>(h);
  }

  template<typename P> raii_inline void operator()(std::coroutine_handle<P> h) const noexcept { h.destroy(); }
};

RAII_NS_END

#endif// COROUTINE_DESTROY_HPP
