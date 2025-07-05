#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"

// LWG 2905 is_constructible_v<unique_ptr<P, D>, P, D const &> should be false when D is not copy constructible

namespace {
template<typename T, typename D, typename... Args> constexpr bool unique_ptr_is_constructible() noexcept
{
  return std::is_constructible_v<raii::unique_ptr<T, D>, Args...>;
}

struct Del
{
  void operator()(void * /*unused*/) const {}
};
}// namespace


TEST_CASE("LWG 2905 test is unique_ptr of int and int[] constructible from int* with trivial deleter",
  "[unique_ptr][unique_ptr::unique_ptr][std::is_constructible_v]")
{
  // Single int
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int, Del &, int *, Del>());

  STATIC_CHECK(unique_ptr_is_constructible<int, Del &, int *, Del &>());

  STATIC_CHECK(unique_ptr_is_constructible<int, Del const &, int *, Del &>());

  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int, Del &, int *, Del const &>());

  STATIC_CHECK(unique_ptr_is_constructible<int, Del, int *, Del const &>());

  STATIC_CHECK(unique_ptr_is_constructible<int, Del, int *, Del>());


  // Array of ints
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int[], Del &, int *, Del>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK(unique_ptr_is_constructible<int[], Del &, int *, Del &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK(unique_ptr_is_constructible<int[], Del const &, int *, Del &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int[], Del &, int *, Del const &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK(unique_ptr_is_constructible<int[], Del, int *, Del const &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK(unique_ptr_is_constructible<int[], Del, int *, Del>());
}

namespace {
struct DelNoCopy
{
  DelNoCopy() = default;
  DelNoCopy(const DelNoCopy &) = delete;
  DelNoCopy(DelNoCopy &&) = default;

  DelNoCopy &operator=(const DelNoCopy &) = default;
  DelNoCopy &operator=(DelNoCopy &&) = default;

  ~DelNoCopy() = default;

  void operator()(void * /*unused*/) const {}
};
}// namespace

TEST_CASE("LWG 2905 test is unique_ptr of int and int[] constructible from int* with deleted copy constructor deleter",
  "[unique_ptr][unique_ptr::unique_ptr][std::is_constructible_v]")
{
  // Single int
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int, DelNoCopy &, int *, DelNoCopy>());

  STATIC_CHECK(unique_ptr_is_constructible<int, DelNoCopy &, int *, DelNoCopy &>());

  STATIC_CHECK(unique_ptr_is_constructible<int, DelNoCopy const &, int *, DelNoCopy &>());

  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int, DelNoCopy &, int *, DelNoCopy const &>());

  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int, DelNoCopy, int *, DelNoCopy const &>());

  STATIC_CHECK(unique_ptr_is_constructible<int, DelNoCopy, int *, DelNoCopy>());


  // Array of ints
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int[], DelNoCopy &, int *, DelNoCopy>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK(unique_ptr_is_constructible<int[], DelNoCopy &, int *, DelNoCopy &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK(unique_ptr_is_constructible<int[], DelNoCopy const &, int *, DelNoCopy &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int[], DelNoCopy &, int *, DelNoCopy const &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int[], DelNoCopy, int *, DelNoCopy const &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK(unique_ptr_is_constructible<int[], DelNoCopy, int *, DelNoCopy>());
}

namespace {
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct Base
{
  Base() = default;
  Base(const Base &) = default;
  Base(Base &&) = default;

  Base &operator=(const Base &) = default;
  Base &operator=(Base &&) = default;

  virtual ~Base() = default;
};

struct Derived : Base
{
};
}// namespace


TEST_CASE("LWG 2905 test is unique_ptr of Base[] constructible from Base* and Derived* with trivial deleter",
  "[unique_ptr][unique_ptr::unique_ptr][std::is_constructible_v]")
{
  // From base Base*

  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base[], Del &, Base *, Del>());


  STATIC_CHECK(unique_ptr_is_constructible<Base[], Del &, Base *, Del &>());

  STATIC_CHECK(unique_ptr_is_constructible<Base[], Del const &, Base *, Del &>());

  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base[], Del &, Base *, Del const &>());

  STATIC_CHECK(unique_ptr_is_constructible<Base[], Del, Base *, Del const &>());

  STATIC_CHECK(unique_ptr_is_constructible<Base[], Del, Base *, Del>());


  // From derived Derived*
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base[], Del &, Derived *, Del>());

  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base[], Del &, Derived *, Del &>());

  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base[], Del const &, Derived *, Del &>());

  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base[], Del &, Derived *, Del const &>());

  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base[], Del, Derived *, Del const &>());

  STATIC_CHECK_FALSE(unique_ptr_is_constructible<Base[], Del, Derived *, Del>());

  // NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
}