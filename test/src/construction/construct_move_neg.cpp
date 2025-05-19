#include <catch2/catch_test_macros.hpp>


#include "memory_delete.hpp"
#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

// LWG 2905 is_constructible_v<unique_ptr<P, D>, P, D const &> should be false when D is not copy constructible

namespace {
template<typename T, typename D, typename... Args> constexpr bool unique_ptr_is_constructible() noexcept
{
  return std::is_constructible_v<raii::unique_ptr<T, D>, Args...>;
}

template<typename T, typename D, typename U> constexpr bool unique_ptr_is_assignable() noexcept
{
  return std::is_assignable_v<raii::unique_ptr<T, D>, U>;
}

struct Base
{
  Base() = default;
  Base(const Base &) = default;
  Base(Base &&) = default;

  Base &operator=(const Base &) = default;
  Base &operator=(Base &&) = default;

  virtual ~Base() = default;
};

struct Derived : public Base
{
};

// 20.7.1.3 unique_ptr for array objects [unique.ptr.runtime]

struct DelArray
{
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  template<typename T> void operator()(const T *ptr) const noexcept { delete[] ptr; }
};
}// namespace


TEST_CASE("is_constructible unique_ptr of array of base objects from unique_ptr of array of derived objects",
  "[unique_ptr][unique_ptr::unique_ptr][std::is_constructible_v]")
{
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<DelArray>,
    raii::unique_ptr<Derived[], raii::deleter_class_wrapper<DelArray>> &&>());
  STATIC_CHECK_FALSE(unique_ptr_is_assignable<Base[],
    raii::deleter_class_wrapper<DelArray>,
    raii::unique_ptr<Derived[], raii::deleter_class_wrapper<DelArray>> &&>());
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
}

// Conversion from non-array form of unique_ptr
TEST_CASE("is_constructible unique_ptr of array of base objects from unique_ptr of single derived object",
  "[unique_ptr][unique_ptr::unique_ptr][std::is_constructible_v]")
{
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base[], raii::default_delete<Base[]>, raii::unique_ptr<Base> &&>());
  STATIC_CHECK_FALSE(unique_ptr_is_assignable<Base[], raii::default_delete<Base[]>, raii::unique_ptr<Base> &&>());
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
}

// Conversion to non-array form of unique_ptr
TEST_CASE("is_constructible unique_ptr of single base object from unique_ptr of array of base objects",
  "[unique_ptr][unique_ptr::unique_ptr][std::is_constructible_v]")
{
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base, raii::default_delete<Base>, raii::unique_ptr<Base[]> &&>());
  STATIC_CHECK_FALSE(unique_ptr_is_assignable<Base, raii::default_delete<Base>, raii::unique_ptr<Base[]> &&>());
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
}