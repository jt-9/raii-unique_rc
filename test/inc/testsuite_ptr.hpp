#ifndef RAII_TESTSUITE_POINTER_HPP
#define RAII_TESTSUITE_POINTER_HPP

#pragma once

#include <compare>// std::compare_three_way_result_t
#include <cstddef>// std::nullptr_t
#include <iterator>


namespace raii_test {

// Copied from gcc's implementation from testsuite_allocator.h

// A class type meeting *only* the Cpp17NullablePointer requirements.
// Can be used as a base class for fancy pointers (like PointerBase, below)
// or to wrap a built-in pointer type to remove operations not required
// by the Cpp17NullablePointer requirements (dereference, increment etc.)

template<typename Ptr> struct NullablePointer
{
  // N.B. default constructor does not initialize value
  // cppcheck-suppress [noExplicitConstructor, uninitMemberVar]
  constexpr NullablePointer() = default;
  // cppcheck-suppress noExplicitConstructor
  constexpr NullablePointer(std::nullptr_t) noexcept : value{} {}

  explicit constexpr operator bool() const noexcept { return value != nullptr; }

  friend inline constexpr bool operator==(NullablePointer lhs, NullablePointer rhs) noexcept
  {
    return lhs.value == rhs.value;
  }

protected:
  explicit constexpr NullablePointer(Ptr ptr) noexcept : value{ ptr } {}
  Ptr value;
};

// NullablePointer<void> is an empty type that models Cpp17NullablePointer.
template<> struct NullablePointer<void>
{
  constexpr NullablePointer() = default;
  // cppcheck-suppress noExplicitConstructor
  constexpr NullablePointer(std::nullptr_t) noexcept {}
  explicit constexpr NullablePointer(const volatile void *) noexcept {}

  explicit constexpr operator bool() const noexcept { return false; }

  friend inline constexpr bool operator==(NullablePointer, NullablePointer) noexcept { return true; }
};

// Utility for use as CRTP base class of custom pointer types
template<typename Derived, typename T> struct PointerBase : NullablePointer<T *>
{
  using element_type = T;

  // typedefs for iterator_traits
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;
  using pointer = Derived;
  using reference = T &;

  using NullablePointer<T *>::NullablePointer;

  // Public (but explicit) constructor from raw pointer:
  explicit constexpr PointerBase(T *p) noexcept : NullablePointer<T *>(p) {}

  template<typename D, typename U, typename = decltype(static_cast<T *>(std::declval<U *>()))>
  // cppcheck-suppress noExplicitConstructor
  constexpr PointerBase(const PointerBase<D, U> &p) : NullablePointer<T *>(p.operator->())
  {}

  constexpr T &operator*() const { return *this->value; }
  constexpr T *operator->() const { return this->value; }
  constexpr T &operator[](difference_type n) const { return this->value[n]; }

  constexpr Derived &operator++()
  {
    ++this->value;
    return derived();
  }

  constexpr Derived &operator--()
  {
    --this->value;
    return derived();
  }

  constexpr Derived operator++(int) { return Derived(this->value++); }

  constexpr Derived operator--(int) { return Derived(this->value--); }

  constexpr Derived &operator+=(difference_type n)
  {
    this->value += n;
    return derived();
  }

  constexpr Derived &operator-=(difference_type n)
  {
    this->value -= n;
    return derived();
  }

  constexpr Derived operator+(difference_type n) const
  {
    Derived p(derived());
    return p += n;
  }

  constexpr Derived operator-(difference_type n) const
  {
    Derived p(derived());
    return p -= n;
  }

private:
  friend constexpr std::ptrdiff_t operator-(PointerBase l, PointerBase r) noexcept { return l.value - r.value; }

  friend constexpr auto operator<=>(PointerBase l, PointerBase r) noexcept { return l.value <=> r.value; }

  constexpr Derived &derived() noexcept { return static_cast<Derived &>(*this); }

  constexpr const Derived &derived() const noexcept { return static_cast<const Derived &>(*this); }
};

// implementation for pointer-to-void specializations
template<typename T> struct PointerBase_void : NullablePointer<T *>
{
  using element_type = T;

  // typedefs for iterator_traits
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;

  using NullablePointer<T *>::NullablePointer;

  constexpr T *operator->() const { return this->value; }

  template<typename D, typename U, typename = decltype(static_cast<T *>(std::declval<U *>()))>
  // cppcheck-suppress noExplicitConstructor
  constexpr PointerBase_void(const PointerBase<D, U> &p) : NullablePointer<T *>(p.operator->())
  {}
};

template<typename Derived> struct PointerBase<Derived, void> : PointerBase_void<void>
{
  using PointerBase_void::PointerBase_void;
  using pointer = Derived;
};

template<typename Derived> struct PointerBase<Derived, const void> : PointerBase_void<const void>
{
  using PointerBase_void::PointerBase_void;
  using pointer = Derived;
};

}// namespace raii_test

#endif// RAII_TESTSUITE_POINTER_HPP