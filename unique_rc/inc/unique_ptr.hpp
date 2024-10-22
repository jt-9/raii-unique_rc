#ifndef UNIQUE_PTR_HPP
#define UNIQUE_PTR_HPP

#include "memory_delete.hpp"
#include "unique_rc.hpp"

#include "raii_defs.hpp"

#include <cassert>
#include <concepts>

RAII_NS_BEGIN


template<typename T, typename Deleter = raii::default_delete<T>> class unique_ptr : public unique_rc<T *, Deleter>
{
private:
  using Base = unique_rc<T *, Deleter>;
  using typename Base::handle;
  using Base::Base;
  using Base::operator=;

public:
  using pointer = typename Base::handle;
  using element_type = T;
  using typename Base::deleter_type;
  using invalid_pointer_type = Base::invalid_handle_type;

  using Base::invalid;

private:
  // helper template for detecting a safe conversion from another
  // unique_ptr
  template<typename U, typename D>
  using safe_conversion_from =
    std::conjunction<std::is_convertible<typename unique_ptr<U, D>::pointer, pointer>, std::negation<std::is_array<U>>>;

public:
  raii_inline constexpr unique_ptr() noexcept
    requires is_not_pointer_default_constructable_v<Deleter>
    : Base()
  {}

  template<typename D = Deleter>
  raii_inline explicit constexpr unique_ptr(pointer p) noexcept
    requires is_not_pointer_default_constructable_v<D>
    : Base(p)
  {}

  template<typename D = Deleter>
    requires std::is_copy_constructible_v<D>
  raii_inline constexpr unique_ptr(pointer p, const Deleter &d) noexcept : Base(p, d)
  {}

  template<typename D = Deleter>
    requires std::conjunction_v<std::negation<std::is_reference<D>>, std::is_move_constructible<D>>
  raii_inline constexpr unique_ptr(pointer p, Deleter &&d) noexcept : Base(p, std::move(d))
  {}

  template<typename D = Deleter>
    requires std::conjunction_v<std::is_reference<D>, std::is_constructible<D, std::remove_reference_t<D>>>
  unique_ptr(pointer, std::remove_reference_t<Deleter> &&) = delete;

  // template<typename D>
  // raii_inline constexpr unique_ptr(pointer h, D&& d) noexcept requires std::constructible_from<deleter_type, D>
  //	: uh_{ h, std::forward<D>(d) }
  //{}

  constexpr unique_ptr(unique_ptr &&) = default;

  // Converting constructor from another type
  template<typename U, typename D>
    requires std::conjunction_v<safe_conversion_from<U, D>,
      std::conditional_t<std::is_reference_v<Deleter>, std::is_same<D, Deleter>, std::is_convertible<D, Deleter>>>
  // cppcheck-suppress noExplicitConstructor intended converting constructor
  raii_inline constexpr unique_ptr(unique_ptr<U, D> &&src) noexcept
    : Base(src.release(), std::forward<D>(src.get_deleter()))
  {}

  constexpr unique_ptr &operator=(unique_ptr &&) = default;

  // Assignment from another type
  template<typename U, typename D>
    requires std::conjunction_v<safe_conversion_from<U, D>, std::is_assignable<deleter_type &, D &&>>
  raii_inline constexpr unique_ptr &operator=(unique_ptr<U, D> &&rhs) noexcept
  {
    Base::operator=(std::move(rhs));

    return *this;
  }

  raii_inline constexpr unique_ptr &operator=(std::nullptr_t) noexcept
    requires std::is_same_v<invalid_pointer_type, std::nullptr_t>
  {
    reset();
    return *this;
  }

  unique_ptr(const unique_ptr &) = delete;
  unique_ptr &operator=(const unique_ptr &) = delete;

  constexpr ~unique_ptr() noexcept = default;

  using Base::get;
  using Base::operator->;

  raii_inline constexpr typename std::add_lvalue_reference_t<element_type> operator*() const
    noexcept(noexcept(*std::declval<pointer>()))
  {
    assert(get() != invalid() && "Cannot dereference nullptr");
    return *get();
  }

  using Base::get_deleter;
  using Base::operator bool;

  using Base::reset;
  using Base::release;

  using Base::swap;
};


/// A move-only smart pointer that manages unique ownership of an array.
template<typename T, typename Deleter> class unique_ptr<T[], Deleter> : public unique_rc<T *, Deleter>
{
private:
  using Base = unique_rc<T *, Deleter>;

  using typename Base::handle;
  using Base::Base;
  using Base::operator=;

  // template<typename _Up> using _DeleterConstraint = typename __uniq_ptr_impl<_T, _Up>::_DeleterConstraint::type;
  //  like is_base_of<_T, _Up> but false if unqualified types are the same

  template<typename U>
  using is_derived_T =
    std::conjunction<std::is_base_of<T, U>, std::negation<std::is_same<std::remove_cv_t<T>, std::remove_cv_t<U>>>>;

public:
  using pointer = typename Base::handle;
  using element_type = T;
  using typename Base::deleter_type;
  using invalid_pointer_type = Base::invalid_handle_type;

  using Base::invalid;

  // helper template for detecting a safe conversion from a raw pointer
  template<typename U>
  using safe_conversion_raw =
    std::conjunction<std::disjunction<std::disjunction<std::is_same<U, pointer>, std::is_same<U, std::nullptr_t>>,
      std::conjunction<std::is_pointer<U>,
        std::is_same<pointer, element_type *>,
        std::is_convertible<typename std::remove_pointer<U>::type (*)[], element_type (*)[]>>>>;

  // helper template for detecting a safe conversion from another
  // unique_ptr
  template<typename U,
    typename D,
    typename UP = unique_ptr<U, D>,
    typename UP_pointer = typename UP::pointer,
    typename UP_element_type = typename UP::element_type>
  using safe_conversion_from = std::conjunction<std::is_array<U>,
    std::is_same<pointer, element_type *>,
    std::is_same<UP_pointer, UP_element_type *>,
    std::is_convertible<UP_element_type (*)[], element_type (*)[]>>;

  // Constructors
  raii_inline constexpr unique_ptr() noexcept
    requires is_not_pointer_default_constructable_v<Deleter>
    : Base()
  {}

  template<typename U, typename D = deleter_type>
  raii_inline constexpr explicit unique_ptr(U p) noexcept
    requires std::conjunction_v<is_not_pointer_default_constructable<D>, safe_conversion_raw<U>>
    : Base(p)
  {}

  template<typename U, typename D = deleter_type>
  raii_inline constexpr unique_ptr(U p, const deleter_type &d) noexcept
    requires std::conjunction_v<safe_conversion_raw<U>, std::is_copy_constructible<D>>
    : Base(p, d)
  {}

  template<typename U, typename D = deleter_type>
  raii_inline constexpr unique_ptr(U p, std::enable_if_t<!std::is_lvalue_reference_v<D>, D &&> d) noexcept
    requires std::conjunction_v<safe_conversion_raw<U>, std::is_move_constructible<D>>
    : Base(std::move(p), std::move(d))
  {}

  template<typename U, typename D = deleter_type>
    requires std::conjunction_v<safe_conversion_raw<U>, std::is_lvalue_reference<D>>
  unique_ptr(U, std::remove_reference_t<D> &&) = delete;

  /// Move constructor.
  constexpr unique_ptr(unique_ptr &&) = default;

  /// Creates a unique_ptr that owns nothing.
  template<typename D = Deleter>
    requires is_not_pointer_default_constructable_v<D>
  raii_inline explicit constexpr unique_ptr(std::nullptr_t) noexcept : Base()
  {}

  // Converting constructor from another type
  template<typename U, typename D>
    requires std::conjunction_v<safe_conversion_from<U, D>,
      std::conditional_t<std::is_reference_v<Deleter>, std::is_same<D, Deleter>, std::is_convertible<D, Deleter>>>
  // cppcheck-suppress noExplicitConstructor intended converting constructor
  raii_inline constexpr unique_ptr(unique_ptr<U, D> &&src) noexcept
    : Base(src.release(), std::forward<D>(src.get_deleter()))
  {}

  /// Destructor, invokes the deleter if the stored pointer is not null.
  constexpr ~unique_ptr() noexcept = default;

  constexpr unique_ptr &operator=(unique_ptr &&) = default;

  template<typename U, typename D>
    requires std::conjunction_v<safe_conversion_from<U, D>, std::is_assignable<deleter_type &, D &&>>
  raii_inline constexpr unique_ptr &operator=(unique_ptr<U, D> &&u) noexcept
  {
    Base::operator=(std::move(u));
    return *this;
  }

  raii_inline constexpr unique_ptr &operator=(std::nullptr_t) noexcept
    requires std::is_same_v<invalid_pointer_type, std::nullptr_t>
  {
    reset();
    return *this;
  }

  using Base::get;

  /// Access an element of owned array.
  raii_inline constexpr typename std::add_lvalue_reference_t<element_type> operator[](std::size_t i) const
  {
    assert(get() != invalid() && "Error subscript operator on nullptr");
    return get()[i];
  }

  using Base::get_deleter;
  using Base::operator bool;

  using Base::release;

  // Cannot default p to invalid(), cause U need to be deducable
  template<typename U>
    requires std::disjunction_v<std::is_same<U, pointer>,
      std::conjunction<std::is_same<pointer, element_type *>,
        std::is_pointer<U>,
        std::is_convertible<typename std::remove_pointer_t<U> (*)[], element_type (*)[]>>>
  raii_inline constexpr void reset(U p) noexcept
  {
    Base::reset(std::move(p));
  }

  raii_inline constexpr void reset(std::nullptr_t = nullptr) noexcept
    requires std::is_same_v<invalid_pointer_type, std::nullptr_t>
  {
    Base::reset(invalid());
  }

  using Base::swap;

private:
  // Not applicable for an array, equivalent to *p[0]
  using Base::operator->;
};// unique_ptr<T[], Deleter>


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


// make_unique and make_unique_for_overwrite

template<class T, class... Types>
  requires(!std::is_array_v<T>)
[[nodiscard]] raii_inline constexpr raii::unique_ptr<T> make_unique(Types &&...Args)
{
  return raii::unique_ptr<T>(new T(std::forward<Types>(Args)...));
}

template<class T>
// requires std::is_array_v<T> && (std::extent_v<T> == 0)
  requires std::is_unbounded_array_v<T>
[[nodiscard]] raii_inline constexpr raii::unique_ptr<T> make_unique(const std::size_t size)
{
  using Elem = std::remove_extent_t<T>;
  return raii::unique_ptr<T>(new Elem[size]());
}

template<class T, class... Types>
  requires std::is_bounded_array_v<T>
void make_unique(Types &&...) = delete;

template<typename T>
  requires(!std::is_array_v<T>)
[[nodiscard]] raii_inline constexpr raii::unique_ptr<T> make_unique_for_overwrite()
{
  // with default initialization
  return raii::unique_ptr<T>(new T);
}

template<typename T>
  requires std::is_unbounded_array_v<T>
[[nodiscard]] raii_inline constexpr raii::unique_ptr<T> make_unique_for_overwrite(const std::size_t size)
{
  // only memory allocation
  using Elem = std::remove_extent_t<T>;
  return raii::unique_ptr<T>(new Elem[size]);
}

template<typename T, class... Types>
  requires std::is_bounded_array_v<T>
void make_unique_for_overwrite(Types &&...) = delete;

RAII_NS_END

#endif// UNIQUE_PTR_HPP