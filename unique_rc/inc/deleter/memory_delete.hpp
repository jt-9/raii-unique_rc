// memory_delete for unique_rc
#ifndef MEMORY_DELETE_HPP
#define MEMORY_DELETE_HPP

#include "raii_defs.hpp"

#include <concepts>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>//std::forward

#include "concepts.hpp"

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

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(Handle h) noexcept { return h; }

  raii_inline constexpr void operator()(Handle h) const noexcept
  {
    static_assert(!std::is_void_v<Handle>, "can't delete pointer to incomplete type");
    static_assert(sizeof(Handle) > 0, "can't delete pointer to incomplete type");

    delete h;
  }
};

template<typename T> struct default_delete : public memory_delete<T *>
{
private:
  using Base = memory_delete<T *>;

public:
  constexpr default_delete() noexcept = default;

  template<typename U>
    requires std::is_convertible_v<U *, T *>
  // cppcheck-suppress noExplicitConstructor intended converting constructor
  raii_inline constexpr default_delete(const default_delete<U> &) noexcept
  {}

  using Base::invalid;
  using Base::is_owned;
  using Base::operator();
};

// Specialization of default_delete for arrays, used by 'unique_ptr<T[]>'
template<typename T> struct default_delete<T[]> : public memory_delete<T *>
{
private:
  using Base = memory_delete<T *>;

public:
  constexpr default_delete() noexcept = default;

  template<typename U>
    requires std::is_convertible_v<U (*)[], T (*)[]>
  // cppcheck-suppress noExplicitConstructor intended converting constructor
  raii_inline constexpr default_delete(const default_delete<U[]> &) noexcept
  {}

  using Base::invalid;
  using Base::is_owned;

  template<typename U>
    requires std::is_convertible_v<U (*)[], T (*)[]>
  raii_inline constexpr void operator()(U *p) const noexcept
  {
    static_assert(sizeof(U) > 0, "can't delete pointer to incomplete type");

    delete[] p;
  }
};

template<typename Base>
concept not_final = !std::is_final_v<Base>;

template<typename Deleter>
  requires not_final<Deleter>
struct pointer_deleter_wrapper : public Deleter
{
  using Deleter::Deleter;
  using Deleter::operator=;
  using Deleter::operator();

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  template<typename Handle>
    requires std::is_pointer_v<Handle>
  [[nodiscard]] raii_inline static constexpr bool is_owned(Handle h) noexcept
  {
    return h;
  }
};

template<typename Deleter>
  requires not_final<Deleter> && has_pointer_type<Deleter>
struct pointer_deleter_wrapper<Deleter> : public Deleter
{
  using handle = typename std::remove_reference_t<Deleter>::pointer;

  using Deleter::Deleter;
  using Deleter::operator=;
  using Deleter::operator();

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  template<typename Handle>
    requires std::is_pointer_v<Handle>
  [[nodiscard]] raii_inline static constexpr bool is_owned(Handle h) noexcept
  {
    return h;
  }
};


template<typename Deleter>
  requires std::is_swappable_v<Deleter> && std::swappable<Deleter>
raii_inline constexpr void swap(pointer_deleter_wrapper<Deleter> &lhs, pointer_deleter_wrapper<Deleter> &rhs) noexcept(
  noexcept(std::is_nothrow_swappable_v<Deleter>))
{
  std::ranges::swap(static_cast<Deleter &>(lhs), static_cast<Deleter &>(rhs));
}

template<typename Deleter>
  requires(!std::is_swappable_v<Deleter>)
void swap(pointer_deleter_wrapper<Deleter> &lhs, pointer_deleter_wrapper<Deleter> &rhs) = delete;

RAII_NS_END

#endif// MEMORY_DELETE_HPP
