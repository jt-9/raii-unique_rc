// memory_delete for unique_rc
#ifndef MEMORY_DELETE_HPP
#define MEMORY_DELETE_HPP

#include "raii_defs.hpp"

#include <type_traits>// std::is_pointer_v


RAII_NS_BEGIN

template<typename Handle>
  requires std::is_pointer_v<Handle>
struct memory_delete
{
  constexpr memory_delete() noexcept = default;

  template<typename U>
    requires std::is_convertible_v<U, Handle>
  
  raii_inline explicit constexpr memory_delete(const memory_delete<U> & /*unused*/) noexcept
  {}

#ifdef __cpp_static_call_operator
  // False poisitive, guarded by feature #ifdef __cpp_static_call_operator
  // NOLINTNEXTLINE(clang-diagnostic-c++23-extensions)
  raii_inline static constexpr void operator()(Handle ptr) noexcept
#else
  raii_inline constexpr void operator()(Handle ptr) const noexcept
#endif
  {
    static_assert(!std::is_void_v<std::remove_pointer_t<Handle>>, "can't delete pointer to incomplete type");
    // cppcheck-suppress sizeofVoid;
    // NOLINTNEXTLINE(bugprone-sizeof-expression)
    static_assert(sizeof(std::remove_pointer_t<Handle>) > 0, "can't delete pointer to incomplete type");

    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete ptr;
  }
};

RAII_NS_END

#endif// MEMORY_DELETE_HPP
