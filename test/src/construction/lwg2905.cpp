#include <catch2/catch_test_macros.hpp>


#include "memory_delete.hpp"
#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

// LWG 2905 is_constructible_v<unique_ptr<P, D>, P, D const &> should be false when D is not copy constructible

namespace {
template<typename T, typename D, typename P, typename E> constexpr bool unique_ptr_is_constructible() noexcept
{
  return std::is_constructible_v<raii::unique_ptr<T, D>, P, E>;
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
  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<Del> &,
    int *,
    raii::deleter_class_wrapper<Del>>());

  STATIC_REQUIRE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<Del> &,
    int *,
    raii::deleter_class_wrapper<Del> &>());

  STATIC_REQUIRE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<Del> const &,
    int *,
    raii::deleter_class_wrapper<Del> &>());

  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<Del> &,
    int *,
    raii::deleter_class_wrapper<Del> const &>());

  STATIC_REQUIRE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<Del>,
    int *,
    raii::deleter_class_wrapper<Del> const &>());

  STATIC_REQUIRE(
    unique_ptr_is_constructible<int, raii::deleter_class_wrapper<Del>, int *, raii::deleter_class_wrapper<Del>>());


  // Array of ints
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<Del> &,
    int *,
    raii::deleter_class_wrapper<Del>>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<Del> &,
    int *,
    raii::deleter_class_wrapper<Del> &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<Del> const &,
    int *,
    raii::deleter_class_wrapper<Del> &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<Del> &,
    int *,
    raii::deleter_class_wrapper<Del> const &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<Del>,
    int *,
    raii::deleter_class_wrapper<Del> const &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<Del>,
    int *,
    raii::deleter_class_wrapper<Del>>());
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
  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<DelNoCopy> &,
    int *,
    raii::deleter_class_wrapper<DelNoCopy>>());

  STATIC_REQUIRE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<DelNoCopy> &,
    int *,
    raii::deleter_class_wrapper<DelNoCopy> &>());

  STATIC_REQUIRE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<DelNoCopy> const &,
    int *,
    raii::deleter_class_wrapper<DelNoCopy> &>());

  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<DelNoCopy> &,
    int *,
    raii::deleter_class_wrapper<DelNoCopy> const &>());

  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<DelNoCopy>,
    int *,
    raii::deleter_class_wrapper<DelNoCopy> const &>());

  STATIC_REQUIRE(unique_ptr_is_constructible<int,
    raii::deleter_class_wrapper<DelNoCopy>,
    int *,
    raii::deleter_class_wrapper<DelNoCopy>>());


  // Array of ints
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<DelNoCopy> &,
    int *,
    raii::deleter_class_wrapper<DelNoCopy>>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<DelNoCopy> &,
    int *,
    raii::deleter_class_wrapper<DelNoCopy> &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<DelNoCopy> const &,
    int *,
    raii::deleter_class_wrapper<DelNoCopy> &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<DelNoCopy> &,
    int *,
    raii::deleter_class_wrapper<DelNoCopy> const &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<DelNoCopy>,
    int *,
    raii::deleter_class_wrapper<DelNoCopy> const &>());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_REQUIRE(unique_ptr_is_constructible<int[],
    raii::deleter_class_wrapper<DelNoCopy>,
    int *,
    raii::deleter_class_wrapper<DelNoCopy>>());
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
  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del> &,
    Base *,
    raii::deleter_class_wrapper<Del>>());


  STATIC_REQUIRE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del> &,
    Base *,
    raii::deleter_class_wrapper<Del> &>());

  STATIC_REQUIRE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del> const &,
    Base *,
    raii::deleter_class_wrapper<Del> &>());

  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del> &,
    Base *,
    raii::deleter_class_wrapper<Del> const &>());

  STATIC_REQUIRE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del>,
    Base *,
    raii::deleter_class_wrapper<Del> const &>());

  STATIC_REQUIRE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del>,
    Base *,
    raii::deleter_class_wrapper<Del>>());


  // From derived Derived*
  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del> &,
    Derived *,
    raii::deleter_class_wrapper<Del>>());

  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del> &,
    Derived *,
    raii::deleter_class_wrapper<Del> &>());

  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del> const &,
    Derived *,
    raii::deleter_class_wrapper<Del> &>());

  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del> &,
    Derived *,
    raii::deleter_class_wrapper<Del> const &>());

  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del>,
    Derived *,
    raii::deleter_class_wrapper<Del> const &>());

  STATIC_REQUIRE_FALSE(unique_ptr_is_constructible<Base[],
    raii::deleter_class_wrapper<Del>,
    Derived *,
    raii::deleter_class_wrapper<Del>>());

  // NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
}