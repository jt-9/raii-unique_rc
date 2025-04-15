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


TEST_CASE("Compare empty arrays unique_ptr", "[unique_ptr comparisons]")
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

TEST_CASE("Compare empty with allocated array unique_ptr", "[unique_ptr comparisons]")
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

TEST_CASE("Equality of value initialised unique_rc<double*, memory_delete<double*>>", "[unique_rc operator ==]")
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

TEST_CASE("Three-way value initialised unique_rc<int*, memory_delete<int*>>", "[unique_rc operator <=>]")
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