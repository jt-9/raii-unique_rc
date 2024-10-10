#ifndef UNIQUE_PTR_HPP
#define UNIQUE_PTR_HPP

#include "unique_rc.hpp"

#include "defs.hpp"

#include <cassert>
#include <concepts>

RAII_NS_BEGIN


template<typename Tp> struct default_delete
{
  constexpr default_delete() noexcept = default;

  template<typename U>
  raii_inline constexpr default_delete(const default_delete<U> &) noexcept
    requires std::is_convertible_v<U *, Tp *>
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(Tp *p) const noexcept
  {
    static_assert(!std::is_void_v<Tp>, "can't delete pointer to incomplete type");
    static_assert(sizeof(Tp) > 0, "can't delete pointer to incomplete type");

    delete p;
  }
};

// Specialization of default_delete for arrays, used by `unique_ptr<T[]>`
template<typename Tp> struct default_delete<Tp[]>
{
  constexpr default_delete() noexcept = default;

  template<typename U>
  raii_inline constexpr default_delete(const default_delete<U[]> &) noexcept
    requires std::is_convertible_v<U (*)[], Tp (*)[]>
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  template<typename U>
  raii_inline constexpr void operator()(U *p) const noexcept
    requires std::is_convertible_v<U (*)[], Tp (*)[]>
  {
    static_assert(sizeof(Tp) > 0, "can't delete pointer to incomplete type");

    delete[] p;
  }
};


template<typename Tp, typename Deleter = raii::default_delete<Tp>> class unique_ptr : private unique_rc<Tp *, Deleter>
{
private:
  using Base = unique_rc<Tp *, Deleter>;

public:
  using pointer = typename Base::handle;
  using element_type = Tp;
  using typename Base::deleter_type;
  using invalid_pointer_type = Base::invalid_handle_type;

  using Base::invalid;

private:
  // helper template for detecting a safe conversion from another
  // unique_ptr
  template<typename H, typename D>
  using safe_conversion_from =
    std::conjunction<std::is_convertible<typename unique_ptr<H, D>::pointer, pointer>, std::negation<std::is_array<H>>>;

public:
  raii_inline constexpr unique_ptr() noexcept : Base() {}

  template<typename D = Deleter>
  raii_inline explicit constexpr unique_ptr(pointer h) noexcept
    requires is_not_pointer_default_constructable_v<D>
    : Base{ h }
  {}

  template<typename D = Deleter>
  raii_inline constexpr unique_ptr(pointer h, const Deleter &d) noexcept
    requires std::is_copy_constructible_v<D>
    : Base{ h, d }
  {}

  template<typename D = Deleter>
  raii_inline constexpr unique_ptr(pointer h, Deleter &&d) noexcept
    requires std::conjunction_v<std::negation<std::is_reference<D>>, std::is_move_constructible<D>>
    : Base{ h, std::move(d) }
  {}

  template<typename D = Deleter,
    std::enable_if_t<std::conjunction_v<std::is_reference<D>, std::is_constructible<D, std::remove_reference_t<D>>>,
      int> = 0>
  unique_ptr(pointer, std::remove_reference_t<Deleter> &&) = delete;

  // template<typename D>
  // raii_inline constexpr unique_ptr(pointer h, D&& d) noexcept requires std::constructible_from<deleter_type, D>
  //	: uh_{ h, std::forward<D>(d) }
  //{}

  constexpr unique_ptr(unique_ptr &&) = default;

  // Converting constructor from another type
  template<typename H, typename D>
  raii_inline constexpr unique_ptr(unique_ptr<H, D> &&src) noexcept
    requires std::conjunction_v<safe_conversion_from<H, D>,
      std::conditional_t<std::is_reference_v<Deleter>, std::is_same<D, Deleter>, std::is_convertible<D, Deleter>>>
    : Base{ src.release(), std::forward<D>(src.get_deleter()) }
  {}

  constexpr unique_ptr &operator=(unique_ptr &&) = default;

  // Assignment from another type
  template<typename H, typename D>
  raii_inline constexpr unique_ptr &operator=(unique_ptr<H, D> &&rhs) noexcept
    requires std::conjunction_v<safe_conversion_from<H, D>, std::is_assignable<deleter_type &, D &&>>
  {
    Base::operator=(std::move(rhs));

    return *this;
  }

  unique_ptr(const unique_ptr &) = delete;
  unique_ptr &operator=(const unique_ptr &) = delete;

  raii_inline constexpr ~unique_ptr() noexcept = default;

  using Base::get;
  using Base::operator->;

  raii_inline constexpr typename std::add_lvalue_reference_t<element_type> operator*() const
    noexcept(noexcept(*std::declval<pointer>()))
  {
    assert(get() != invalid() && "Cannot dereference nullptr");
    return *get();
  }

  using Base::reset;
  using Base::release;

  using Base::get_deleter;
  using Base::operator bool;

  using Base::swap;
};


template<typename T1, class D1, typename T2, class D2>
  requires std::equality_comparable_with<T1, T2>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_ptr<T1, D1> &lhs,
  const unique_ptr<T2, D2> &rhs) noexcept(noexcept(lhs.get() == rhs.get()))
{
  return lhs.get() == rhs.get();
}

// unique_ptr comparison with nullptr
template<typename T, typename D>
[[nodiscard]] raii_inline constexpr bool operator==(const unique_ptr<T, D> &r, std::nullptr_t) noexcept
{
  return !r;
}

template<typename T1, class D1, typename T2, class D2>
  requires std::three_way_comparable_with<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<typename unique_ptr<T1, D1>::pointer,
  typename unique_ptr<T2, D2>::pointer>
  operator<=>(const unique_ptr<T1, D1> &lhs, const unique_ptr<T2, D2> &rhs) noexcept(noexcept(lhs.get() <=> rhs.get()))
{
  return lhs.get() <=> rhs.get();
}

template<typename T, typename D>
  requires std::three_way_comparable<typename unique_ptr<T, D>::pointer>
[[nodiscard]] raii_inline constexpr std::compare_three_way_result_t<typename unique_ptr<T, D>::pointer>
  operator<=>(const unique_ptr<T, D> &r, std::nullptr_t) noexcept(noexcept(r.get()))
{
  using pointer = typename unique_ptr<T, D>::pointer;
  return r.get() <=> static_cast<pointer>(nullptr);
}

template<typename H, typename D>
  requires std::is_swappable_v<typename unique_ptr<H, D>::pointer>
raii_inline constexpr void swap(unique_ptr<H, D> &lhs, unique_ptr<H, D> &rhs) noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

RAII_NS_END

#endif// UNIQUE_PTR_HPP