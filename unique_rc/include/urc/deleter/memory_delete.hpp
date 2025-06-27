// memory_delete for unique_rc
#ifndef MEMORY_DELETE_HPP
#define MEMORY_DELETE_HPP

#include "../raii_defs.hpp"

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

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(Handle h) noexcept { return h; }

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


template<typename Base>
concept not_final = !std::is_final_v<Base>;

template<typename Deleter>
  requires not_final<Deleter>
struct deleter_class_wrapper : public Deleter
{
  using Deleter::Deleter;
  using Deleter::operator=;
  using Deleter::operator();

  // cppcheck-suppress duplInheritedMember false positive mixed std::default_delete and raii::default_delete
  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  template<typename Handle>
    requires std::is_pointer_v<Handle>
  [[nodiscard]] raii_inline static constexpr bool is_owned(Handle h) noexcept
  {
    return static_cast<bool>(h);
  }
};

template<typename Deleter>
  requires std::is_swappable_v<Deleter> && std::swappable<Deleter>
raii_inline constexpr void swap(deleter_class_wrapper<Deleter> &lhs, deleter_class_wrapper<Deleter> &rhs) noexcept(
  noexcept(std::is_nothrow_swappable_v<Deleter>))
{
  std::ranges::swap(static_cast<Deleter &>(lhs), static_cast<Deleter &>(rhs));
}

template<typename Deleter>
  requires(!std::is_swappable_v<Deleter>)
void swap(deleter_class_wrapper<Deleter> &lhs, deleter_class_wrapper<Deleter> &rhs) = delete;


// template<typename Deleter, Deleter deleter_func_ptr>
//   requires std::is_function_v<std::remove_pointer_t<Deleter>>
// struct deleter_func_ptr_wrapper
// {
//   [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

//   template<typename Handle>
//     requires std::is_pointer_v<Handle>
//   [[nodiscard]] raii_inline static constexpr bool is_owned(Handle h) noexcept
//   {
//     return h;
//   }

//   template<typename Handle>
//     requires std::is_pointer_v<Handle>
//   raii_inline constexpr void operator()(Handle h) const noexcept
//   {
//     deleter_func_ptr(h);
//   }
// };

RAII_NS_END

#endif// MEMORY_DELETE_HPP
