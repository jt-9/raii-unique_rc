#include <catch2/catch_test_macros.hpp>


#include "memory_delete.hpp"
#include "unique_ptr.hpp"
#include "unique_rc.hpp"

#include <concepts>
#include <cstddef>//std::nullptr_t
#include <type_traits>//std::is_swappable_v
#include <utility>
// #include <memory>


TEST_CASE("Member swap value initialised unique_rc<int*, memory_delete<int*>>", "[unique_rc][unique_rc::swap][swap]")
{
  const auto rc1_init_number = 24;
  raii::unique_rc<int *, raii::memory_delete<int *>> rc1{ new int{ rc1_init_number } };

  CHECK(rc1);

  SECTION("unique_rc::swap with other value constructed unique_rc")
  {
    const auto rc2_init_number = -36;
    using handle = decltype(rc1)::handle;
    raii::unique_rc<handle, raii::memory_delete<handle>> rc2{ new int{ rc2_init_number } };
    CHECK(rc2);

    auto *const ptr1 = rc1.get();
    auto *const ptr2 = rc2.get();

    rc1.swap(rc2);

    CHECK(rc1);
    CHECK(rc2);

    REQUIRE(ptr2 == rc1.get());
    REQUIRE(ptr1 == rc2.get());
  }

  SECTION("unique_rc::swap with default constructed unique_rc")
  {
    using handle = decltype(rc1)::handle;
    raii::unique_rc<handle, raii::memory_delete<handle>> default_init_rc{};
    CHECK_FALSE(default_init_rc);

    auto *const ptr1 = rc1.get();
    auto *const ptr2 = default_init_rc.get();

    rc1.swap(default_init_rc);

    REQUIRE(ptr2 == rc1.get());
    REQUIRE(ptr1 == default_init_rc.get());
  }
}

TEST_CASE("Swap unique_ptr with other unique_ptr", "[unique_ptr][std::swap][swap]")
{
  struct A
  {
  };

  // NOLINTBEGIN(readability-identifier-length)
  raii::unique_ptr<A> p1;

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, clang-analyzer-cplusplus.NewDeleteLeaks)
  raii::unique_ptr<A> p2{ new A };
  raii::unique_ptr<A> p3;
  // NOLINTEND(readability-identifier-length)

  std::swap(p3, p2);

  REQUIRE(p1 != p3);
  REQUIRE(p2 != p3);
  REQUIRE(p1 == p2);
}


namespace {

// NOLINTBEGIN(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct B
{
  static constexpr std::nullptr_t invalid() noexcept;
  static constexpr bool is_owned(void *) noexcept;

  void operator()([[maybe_unused]] void *ptr) const {}
};
// NOLINTEND(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)

void swap(B &, B &) = delete;

// NOLINTBEGIN(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct C
{
  void operator()([[maybe_unused]] void *ptr) const {}
};
// NOLINTEND(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)

void swap(C &, C &) = delete;

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct D
{
  D(D &&) = delete;

  void operator()([[maybe_unused]] void *ptr) const {}
};

struct NoSwapPtr
{
  // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
  struct pointer
  {
    int &operator*() && noexcept;// this is used by unique_ptr
    int &operator*() const & = delete;// this should not be

    int *operator->() noexcept(false);// noexcept here doesn't affect anything

    // Needed for NullablePointer requirements
    explicit pointer(int * = nullptr);
    pointer(pointer &&) = delete;

    bool operator==(const pointer &) const noexcept;
    bool operator!=(const pointer &) const noexcept;
  };

  static constexpr pointer invalid() noexcept;
  static constexpr bool is_owned(pointer) noexcept;

  void operator()([[maybe_unused]] pointer ptr) const noexcept {}
};

}// namespace

TEST_CASE("Swap static test raii::unique_ptr not swappable via the generic std::swap",
  "[unique_ptr][std::is_swappable][swap]")
{
  // Not swappable, and unique_ptr not swappable via the generic std::swap.
  STATIC_REQUIRE_FALSE(std::is_swappable_v<raii::unique_ptr<int, B>>);
  STATIC_REQUIRE_FALSE(std::is_swappable_v<raii::unique_ptr<int, raii::deleter_class_wrapper<C>>>);
  STATIC_REQUIRE_FALSE(std::is_swappable_v<raii::unique_ptr<int, raii::deleter_class_wrapper<D>>>);
}

TEST_CASE("Swap static test raii::unique_rc not swappable via the generic std::swap",
  "[unique_rc][std::is_swappable][swap]")
{
  // Not swappable, and unique_ptr not swappable via the generic std::swap.
  STATIC_REQUIRE_FALSE(std::is_swappable_v<raii::unique_rc<int *, B>>);
  STATIC_REQUIRE_FALSE(std::is_swappable_v<raii::unique_rc<int *, raii::deleter_class_wrapper<C>>>);
  STATIC_REQUIRE_FALSE(std::is_swappable_v<raii::unique_rc<int *, raii::deleter_class_wrapper<D>>>);

  /* The following code doesn't even compile

  using PtrNoSwapURC = raii::unique_rc<NoSwapPtr::pointer, NoSwapPtr>;
  STATIC_REQUIRE_FALSE(std::is_swappable_v<PtrNoSwapURC>);

  compile error: constraint (has_static_invalid_convertible_handle) not satisfied for class template
  'unique_rc' because 'has_static_invalid_convertible_handle<std::remove_reference_t<NoSwapPtr>,
  std::decay_t<pointer> >' evaluated to false because type constraint 'std::convertible_to<(anonymous
  namespace)::NoSwapPtr::pointer, (anonymous namespace)::NoSwapPtr::pointer>' was not satisfied
  */

  // constexpr auto isSwappable = std::is_swappable_v<std::unique_ptr<NoSwapPtr::pointer, NoSwapPtr>>; // returns true,
  // but should be false
}

TEST_CASE("Swap single value constructed unique_ptr", "[unique_ptr][swap]")
{
  using raii::swap;

  raii::unique_ptr<int> ptr1;

  swap(ptr1, ptr1);
  REQUIRE_FALSE(ptr1);

  raii::unique_ptr<int> ptr2;

  swap(ptr1, ptr2);
  REQUIRE_FALSE(ptr1);
  REQUIRE_FALSE(ptr2);

  raii::unique_ptr<int> ptr3{ new int{ 3 } };

  std::ranges::swap(ptr3, ptr3);
  REQUIRE(*ptr3 == 3);

  std::ranges::swap(ptr1, ptr3);
  REQUIRE(*ptr1 == 3);

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  raii::unique_ptr<int> ptr4{ new int{ 4 } };

  std::ranges::swap(ptr4, ptr1);
  REQUIRE(*ptr4 == 3);
  REQUIRE(*ptr1 == 4);

  // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
}

TEST_CASE("Swap array constructed unique_ptr", "[unique_ptr][std::ranges::swap][swap]")
{
  using raii::swap;

  // NOLINTNEXTLINE
  raii::unique_ptr<int[]> ptr_a1, ptr_a2;

  swap(ptr_a1, ptr_a2);
  REQUIRE_FALSE(ptr_a1);
  REQUIRE_FALSE(ptr_a2);

  // NOLINTNEXTLINE
  raii::unique_ptr<int[]> ptr_a3{ new int[]{ 3 } };

  std::ranges::swap(ptr_a1, ptr_a3);
  REQUIRE(ptr_a1[0] == 3);

  // NOLINTNEXTLINE
  raii::unique_ptr<int[]> ptr_a4{ new int[]{ 4, 5 } };

  std::ranges::swap(ptr_a1, ptr_a4);
  REQUIRE(ptr_a1[1] == 5);

  // False positive resources are released in raii::unique_ptr destructor
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
}

TEST_CASE("Swap value initialised unique_ptr", "[unique_ptr][unique_ptr::swap][swap]")
{
  const auto test_number = 289;
  raii::unique_ptr<int> ptr1 = raii::make_unique<int>(test_number);

  CHECK(ptr1);

  SECTION("unique_ptr::swap with other value constructed unique_ptr")
  {
    const auto test_number2 = -36;
    using element_type = decltype(ptr1)::element_type;
    raii::unique_ptr<element_type, decltype(ptr1)::deleter_type> ptr2{ new int{ test_number2 } };
    REQUIRE(ptr2);

    auto *const raw_ptr1 = ptr1.get();
    auto *const raw_ptr2 = ptr2.get();

    ptr1.swap(ptr2);

    CHECK(ptr1);
    CHECK(ptr2);

    REQUIRE(raw_ptr2 == ptr1.get());
    REQUIRE(raw_ptr1 == ptr2.get());
  }

  SECTION("unique_ptr::swap with default constructed unique_ptr")
  {
    using element_type = decltype(ptr1)::element_type;
    raii::unique_ptr<element_type, decltype(ptr1)::deleter_type> ptr2{};
    CHECK_FALSE(ptr2);

    auto *const raw_ptr1 = ptr1.get();
    auto *const raw_ptr2 = ptr2.get();

    ptr1.swap(ptr2);

    REQUIRE(raw_ptr2 == ptr1.get());
    REQUIRE(raw_ptr1 == ptr2.get());
  }
}