#ifndef UNIQUE_COROUTINE_HANDLE
#define UNIQUE_COROUTINE_HANDLE

#include "concepts.hpp"
#include "coroutine_destroy.hpp"
#include "raii_defs.hpp"
#include "unique_rc.hpp"

#include <coroutine>
#include <cstddef>// std::nullptr_t
#include <utility>// std::move


RAII_NS_BEGIN

template<typename Promise, class Deleter = raii::coroutine_destroy>
class unique_coroutine_handle
  : public unique_rc<std::coroutine_handle<Promise>,
      Deleter,
      raii::resolve_handle_type,
      std::nullptr_t,
      raii::coro_invalid_handle_policy>
{
private:
  using Base = unique_rc<std::coroutine_handle<Promise>,
    Deleter,
    raii::resolve_handle_type,
    std::nullptr_t,
    raii::coro_invalid_handle_policy>;

public:
  using typename Base::invalid_handle_policy;

  /// @brief std::coroutine_handle<Promise>
  using typename Base::handle;

  /// @brief Promise, the type of the object managed by this unique_coroutine_handle
  using element_type = Promise;

  /// @brief Deleter, the function object or lvalue reference or to function object, to be called from the destructor
  using typename Base::deleter_type;

  /// @brief Represents invalid handle type, whose value is returned by Deleter::invalid()
  /// is assigned to *this handle, when it goes out of scope, or upon reset(), std::nullptr_t in this case
  using typename Base::invalid_handle;

  /// @brief static method returns invalid handle of type `invalid_handle`, `nullptr` in this case
  using Base::invalid;

private:
  /// @brief Helper template for detecting a safe conversion from another unique_coroutine_handle
  template<typename P, class D>
  using safe_conversion_from =
    std::conjunction<std::is_convertible<typename unique_coroutine_handle<P, D>::handle, handle>,
      std::negation<std::is_array<P>>>;

public:
  // cppcheck-suppress-begin [functionStatic, missingReturn]
  /// @brief Creates a unique_coroutine_handle that owns nothing
  raii_inline constexpr unique_coroutine_handle() noexcept
    requires not_pointer_and_is_default_constructable_v<Deleter>
    // cppcheck-suppress [duplInheritedMember]
    : Base()
  {}

  /// @brief Creates a unique_coroutine_handle that owns nothing
  /// @param std::nullptr_t
  raii_inline constexpr unique_coroutine_handle(std::nullptr_t) noexcept
    requires not_pointer_and_is_default_constructable_v<Deleter>
    // cppcheck-suppress [duplInheritedMember]
    : Base()
  {}

  raii_inline explicit constexpr unique_coroutine_handle(handle hnd) noexcept
    requires not_pointer_and_is_default_constructable_v<Deleter>
    : Base(hnd)
  {}

  raii_inline constexpr unique_coroutine_handle(handle hnd, const Deleter &del) noexcept
    requires std::is_copy_constructible_v<Deleter>
    : Base(hnd, del)
  {}

  raii_inline constexpr unique_coroutine_handle(handle hnd, Deleter &&del) noexcept
    requires std::conjunction_v<std::negation<std::is_reference<Deleter>>, std::is_move_constructible<Deleter>>
    : Base(hnd, std::move(del))
  {}
  // cppcheck-suppress-end [functionStatic, missingReturn]

  template<class D = deleter_type>
    requires std::is_lvalue_reference_v<D>
  unique_coroutine_handle(handle, std::remove_reference_t<D> &&) = delete;

  constexpr unique_coroutine_handle(unique_coroutine_handle && /*src*/) noexcept = default;

  // Converting constructor from another type
  template<typename P, class D>
    requires std::conjunction_v<safe_conversion_from<P, D>,
      std::conditional_t<std::is_reference_v<Deleter>, std::is_same<D, Deleter>, std::is_convertible<D, Deleter>>>
  raii_inline
#ifndef RAII_URC_ENABLE_IMPLICIT_CONVERTING_CONSTRUCTOR
    explicit
#endif
    constexpr unique_coroutine_handle(unique_coroutine_handle<P, D> &&hnd) noexcept
    : Base(std::move(hnd))
  {}

  constexpr unique_coroutine_handle &operator=(unique_coroutine_handle && /*rhs*/) noexcept = default;

  // Assignment from another type
  template<typename P, class D>
    requires std::conjunction_v<safe_conversion_from<P, D>, std::is_assignable<deleter_type &, D &&>>
  raii_inline constexpr unique_coroutine_handle &operator=(unique_coroutine_handle<P, D> &&rhs) noexcept
  {
    Base::operator=(std::move(rhs));

    return *this;
  }

  raii_inline constexpr unique_coroutine_handle &operator=(std::nullptr_t) noexcept
    requires std::is_same_v<invalid_handle, std::nullptr_t>
  {
    reset();

    return *this;
  }

  unique_coroutine_handle(const unique_coroutine_handle &) = delete;
  unique_coroutine_handle &operator=(const unique_coroutine_handle &) = delete;

  constexpr ~unique_coroutine_handle() noexcept = default;

  using Base::get;

  raii_inline constexpr void *address() const noexcept { return get().address(); }

  using Base::get_deleter;
  using Base::operator bool;

  using Base::reset;
  using Base::release;

  using Base::swap;
};// unique_coroutine_handle<Promise, Deleter>


// Compares two unique_coroutine_handle<> values lhs and rhs according to their underlying std::coroutine_handle<>
template<typename P1, class D1, typename P2, class D2>
  requires std::equality_comparable_with<P1, P2>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_coroutine_handle<P1, D1> &lhs,
  const unique_coroutine_handle<P2, D2> &rhs) noexcept(noexcept(lhs.get() == rhs.get()))
{
  // cppcheck-suppress mismatchingContainers - false positive guarded by concept std::equality_comparable_with
  return lhs.get() == rhs.get();
}

// Compares unique_coroutine_handle<> lhs aganst nullptr
template<typename P, class D>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_coroutine_handle<P, D> &lhs, std::nullptr_t) noexcept
{ return !lhs; }

// Compares two unique_coroutine_handle<> values lhs and rhs according to their underlying std::coroutine_handle<>
template<typename P1, class D1, typename P2, class D2>
  requires std::three_way_comparable_with<typename unique_coroutine_handle<P1, D1>::handle,
    typename unique_coroutine_handle<P2, D2>::handle>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<typename unique_coroutine_handle<P1, D1>::handle,
  typename unique_coroutine_handle<P2, D2>::handle>
  operator<=>(const unique_coroutine_handle<P1, D1> &lhs, const unique_coroutine_handle<P2, D2> &rhs) noexcept(
    noexcept(lhs.get() <=> rhs.get()))
{ return lhs.get() <=> rhs.get(); }

template<typename P, class D>
  requires std::three_way_comparable<typename unique_coroutine_handle<P, D>::handle>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<typename unique_coroutine_handle<P, D>::handle>
  operator<=>(const unique_coroutine_handle<P, D> &lhs, std::nullptr_t) noexcept(noexcept(lhs.get()))
{
  using handle = unique_coroutine_handle<P, D>::handle;
  return lhs.get() <=> handle{};
}


template<typename P, class D>
  requires /*std::is_swappable_v<D> ||*/ std::swappable<D>
raii_inline constexpr void swap(unique_coroutine_handle<P, D> &lhs, unique_coroutine_handle<P, D> &rhs) noexcept(
  noexcept(std::is_nothrow_swappable_v<D>))
{ lhs.swap(rhs); }

template<typename P, class D>
  requires std::negation_v<std::is_swappable<D>>
void swap(unique_coroutine_handle<P, D> &lhs, unique_coroutine_handle<P, D> &rhs) = delete;

RAII_NS_END


// std::hash specialization for raii::unique_coroutine_handle.
template<typename P, class D>
struct std::hash<raii::unique_coroutine_handle<P, D>>
  : public raii::detail::unique_rc_hash_base<raii::unique_coroutine_handle<P, D>,
      typename raii::unique_coroutine_handle<P, D>::handle>
{
};

#endif// UNIQUE_COROUTINE_HANDLE
