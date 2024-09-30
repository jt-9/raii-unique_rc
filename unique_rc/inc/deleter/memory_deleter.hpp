// memory_delete for unique_rc
#ifndef MEMORY_DELETER_HPP
#define MEMORY_DELETER_HPP

#include "defs.hpp"

#include <concepts>
#include <cstddef>
#include <new>


RAII_NS_BEGIN

template<typename Handle>
  requires std::is_pointer_v<Handle>
struct memory_delete
{
  using handle = Handle;

  constexpr memory_delete() noexcept = default;

  template<typename U>
  raii_inline constexpr memory_delete(const memory_delete<U> &) noexcept
    requires std::is_convertible_v<U, handle>
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(handle h) const noexcept
  {
    static_assert(!std::is_void_v<handle>, "can't delete pointer to incomplete type");
    static_assert(sizeof(handle) > 0, "can't delete pointer to incomplete type");

    delete h;
  }
};


// Specialization of default_delete for arrays, used by `unique_rc<T[]>`
template<typename Handle> struct memory_delete<Handle[]>
{
  using handle = Handle;

  constexpr memory_delete() noexcept = default;

  template<typename U>
  raii_inline constexpr memory_delete(const memory_delete<U[]> &) noexcept
    requires std::is_convertible_v<U (*)[], handle (*)[]>
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  template<typename U>
  raii_inline constexpr void operator()(U h) const noexcept
    requires std::is_convertible_v<U (*)[], handle (*)[]>
  {
    static_assert(sizeof(handle) > 0, "can't delete pointer to incomplete type");

    delete[] h;
  }
};


RAII_NS_END

#endif// MEMORY_DELETER_HPP
