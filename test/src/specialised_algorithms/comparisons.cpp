#include <catch2/catch_test_macros.hpp>


#include "memory_delete.hpp"
#include "mock_pointer_no_op.hpp"
#include "unique_ptr.hpp"
#include "unique_rc.hpp"

namespace {
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct A
{
  virtual ~A() noexcept = default;
};

struct B : A
{
};

}// namespace


TEST_CASE("Compare empty arrays unique_ptr", "[unique_ptr][comparison]")
{
  // NOLINTNEXTLINE
  constexpr raii::unique_ptr<A[]> ptr1;
  // NOLINTNEXTLINE
  constexpr raii::unique_ptr<A[]> ptr2;

  STATIC_REQUIRE(ptr1 == ptr2);
  STATIC_REQUIRE_FALSE(ptr1 != ptr2);
  STATIC_REQUIRE_FALSE(ptr1 < ptr2);
  STATIC_REQUIRE_FALSE(ptr1 > ptr2);
}

TEST_CASE("Compare empty with allocated array unique_ptr", "[unique_ptr][comparison]")
{
  // NOLINTNEXTLINE
  constexpr raii::unique_ptr<A[]> ptr1;
  // NOLINTNEXTLINE
  raii::unique_ptr<A[]> ptr2{ new A[3] };

  REQUIRE(ptr1 != ptr2);
  REQUIRE_FALSE(ptr1 == ptr2);
  REQUIRE(ptr1 < ptr2);
  REQUIRE_FALSE(ptr1 > ptr2);
  REQUIRE(((ptr1 <= ptr2) && (ptr1 != ptr2)));
  REQUIRE_FALSE(((ptr1 >= ptr2) && (ptr1 != ptr2)));
}

TEST_CASE("Equality of value initialised unique_rc<double*, memory_delete<double*>>", "[unique_rc][operator ==][comparison]")
{
  const auto test_number = 13.11;
  const raii::unique_rc<double *, raii::memory_delete<double *>> rc1{ new double{ test_number } };

  CHECK(rc1);

  SECTION("unique_rc::operator == to other value constructed double*")
  {
    const raii::unique_rc<double *, raii::memory_delete<double *>> rc2{ new double{ 1 - test_number } };
    CHECK(rc2);

    REQUIRE_FALSE(rc1 == rc2);
    REQUIRE(rc1 != rc2);
  }

  SECTION("unique_rc::operator == to same, but non-owning unique_rc")
  {
    const raii::unique_rc<double *, mock_raii::mock_pointer_no_op<double *>> noop_rc{ rc1.get() };

    REQUIRE(rc1 == noop_rc);
    REQUIRE_FALSE(rc1 != noop_rc);
  }

  SECTION("unique_rc::operator == to default constructed unique_rc")
  {
    constexpr raii::unique_rc<double *, raii::memory_delete<double *>> default_init_rc{};
    CHECK_FALSE(default_init_rc);

    REQUIRE(rc1 != default_init_rc);
    REQUIRE(rc1 != nullptr);

    REQUIRE(default_init_rc != rc1);
    REQUIRE(nullptr != rc1);
  }
}

TEST_CASE("Three-way value initialised unique_rc<int*, memory_delete<int*>>", "[unique_rc][operator <=>][comparison]")
{
  const raii::unique_rc<int *, raii::memory_delete<int *>> rc1{ new int{ 73 } };

  CHECK(rc1);

  SECTION("unique_rc::operator <=> with other value constructed unique_rc")
  {
    const raii::unique_rc<int *, raii::memory_delete<int *>> rc2{ new int{ 37 } };
    CHECK(rc2);

    REQUIRE((rc1 <=> rc2) != std::strong_ordering::equal);
  }

  SECTION("unique_rc::operator <=> with same, but non-owning unique_rc")
  {
    const raii::unique_rc<int *, mock_raii::mock_pointer_no_op<int *>> noop_rc{ rc1.get() };

    REQUIRE((rc1 <=> noop_rc) == std::strong_ordering::equal);

    const auto equal_to_zero = (rc1 <=> noop_rc) == 0;
    REQUIRE(equal_to_zero);
  }

  SECTION("unique_rc::operator <=> to default constructed unique_rc")
  {
    constexpr raii::unique_rc<int *, raii::memory_delete<int *>> default_init_rc{};
    CHECK_FALSE(default_init_rc);

    REQUIRE((rc1 <=> default_init_rc) == std::strong_ordering::greater);
    REQUIRE((rc1 <=> nullptr) == std::strong_ordering::greater);

    REQUIRE((default_init_rc <=> rc1) == std::strong_ordering::less);
    REQUIRE((nullptr <=> rc1) == std::strong_ordering::less);
  }
}

TEST_CASE("Equality of value initialised unique_ptr", "[unique_ptr][operator ==][comparison]")
{
  const auto test_number = 13.11;
  const raii::unique_ptr<double> ptr1{ new double{ test_number } };

  CHECK(ptr1);

  SECTION("unique_ptr::operator == to other value constructed double*")
  {
    const raii::unique_ptr<double> ptr2{ new double{ 1 - test_number } };
    CHECK(ptr2);

    REQUIRE_FALSE(ptr1 == ptr2);
    REQUIRE(ptr1 != ptr2);
  }

  SECTION("unique_ptr::operator == to same, but non-owning unique_ptr")
  {
    const raii::unique_ptr<double, mock_raii::mock_pointer_no_op<double *>> noop_ptr{ ptr1.get() };

    REQUIRE(ptr1 == noop_ptr);
    REQUIRE_FALSE(ptr1 != noop_ptr);
  }

  SECTION("unique_ptr::operator == to default constructed unique_ptr")
  {
    constexpr raii::unique_ptr<double> default_initalised{};
    CHECK_FALSE(default_initalised);

    REQUIRE(ptr1 != default_initalised);
    REQUIRE(ptr1 != nullptr);

    REQUIRE(default_initalised != ptr1);
    REQUIRE(nullptr != ptr1);
  }
}

TEST_CASE("Three-way operator <=> with value initialised unique_ptr<int>", "[unique_ptr][operator <=>][comparison]")
{
  const raii::unique_ptr<int> ptr1{ new int{ 73 } };

  CHECK(ptr1);

  SECTION("With other value constructed unique_ptr")
  {
    const raii::unique_ptr<int> ptr2{ new int{ 37 } };
    CHECK(ptr2);

    REQUIRE((ptr1 <=> ptr2) != std::strong_ordering::equal);
  }

  SECTION("With same, but non-owning unique_ptr")
  {
    const raii::unique_ptr<int, mock_raii::mock_pointer_no_op<int *>> noop_ptr{ ptr1.get() };

    REQUIRE((ptr1 <=> noop_ptr) == std::strong_ordering::equal);

    const auto equals_to_zero = (ptr1 <=> noop_ptr) == 0;
    REQUIRE(equals_to_zero);
  }

  SECTION("Default constructed unique_ptr")
  {
    constexpr raii::unique_ptr<int> default_init{};
    CHECK_FALSE(default_init);

    REQUIRE((ptr1 <=> default_init) == std::strong_ordering::greater);
    REQUIRE((ptr1 <=> nullptr) == std::strong_ordering::greater);

    REQUIRE((default_init <=> ptr1) == std::strong_ordering::less);
    REQUIRE((nullptr <=> ptr1) == std::strong_ordering::less);
  }
}