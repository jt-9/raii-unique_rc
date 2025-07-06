// memory_delete for unique_rc
#ifndef MEMORY_DELETE_HPP
#define MEMORY_DELETE_HPP

#include "raii_defs.hpp"

#include <concepts>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>//std::forward


RAII_NS_BEGIN

template<typename Handle>
  requires std::is_pointer_v<Handle>
struct memory_delete
{
  constexpr memory_delete() noexcept = default;

  template<typename U>
    requires std::is_convertible_v<U, Handle>
  // cppcheck-suppress noExplicitConstructor intended converting constructor
  raii_inline constexpr memory_delete(const memory_delete<U> &) noexcept
  {}

#ifdef __cpp_static_call_operator
  // False poisitive, guarded by feature #ifdef __cpp_static_call_operator
  // NOLINTNEXTLINE(clang-diagnostic-c++23-extensions)
  raii_inline static constexpr void operator()(Handle h) noexcept
#else
  raii_inline constexpr void operator()(Handle h) const noexcept
#endif
  {
    static_assert(!std::is_void_v<std::remove_pointer_t<Handle>>, "can't delete pointer to incomplete type");
    static_assert(sizeof(std::remove_pointer_t<Handle>) > 0, "can't delete pointer to incomplete type");

    delete h;
  }
};

RAII_NS_END

#endif// MEMORY_DELETE_HPP
