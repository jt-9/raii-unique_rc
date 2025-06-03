#include <catch2/catch_test_macros.hpp>


// #include "urc/unique_rc.hpp"
#include "urc/unique_ptr.hpp"

#include <compare>


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


TEST_CASE("Compare two empty unique_ptr arrays of type A", "[unique_ptr][compare][constexpr]")
{
  // NOLINTNEXTLINE
  constexpr raii::unique_ptr<A[]> ptr1;
  // NOLINTNEXTLINE
  constexpr raii::unique_ptr<A[]> ptr2;

  STATIC_CHECK(ptr1 == ptr2);
  STATIC_CHECK_FALSE(ptr1 != ptr2);
  STATIC_CHECK_FALSE(ptr1 < ptr2);
  STATIC_CHECK_FALSE(ptr1 > ptr2);
}

// TEST_CASE("Compare empty with allocated array unique_ptr", "[unique_ptr][compare]")
//{
//   // NOLINTNEXTLINE
//   constexpr raii::unique_ptr<A[]> ptr1;
//   // NOLINTNEXTLINE
//   raii::unique_ptr<A[]> ptr2{ new A[3] };
//
//   CHECK(ptr1 != ptr2);
//   CHECK_FALSE(ptr1 == ptr2);
//   CHECK(ptr1 < ptr2);
//   CHECK_FALSE(ptr1 > ptr2);
//   CHECK(((ptr1 <= ptr2) && (ptr1 != ptr2)));
//   CHECK_FALSE(((ptr1 >= ptr2) && (ptr1 != ptr2)));
// }

TEST_CASE("Compare empty unique_ptr of type int with nullptr", "[unique_ptr][compare][constexpr]")
{
  constexpr auto uptr = raii::unique_ptr<int>{ nullptr };

  STATIC_CHECK(uptr == nullptr);
  STATIC_CHECK(nullptr == uptr);

  STATIC_CHECK_FALSE(uptr != nullptr);
  STATIC_CHECK_FALSE(nullptr != uptr);
  STATIC_CHECK_FALSE(uptr < nullptr);
  STATIC_CHECK_FALSE(nullptr < uptr);

  STATIC_CHECK(uptr <= nullptr);
  STATIC_CHECK(nullptr <= uptr);

  STATIC_CHECK_FALSE(uptr > nullptr);
  STATIC_CHECK_FALSE(nullptr > uptr);

  STATIC_CHECK(uptr >= nullptr);
  STATIC_CHECK(nullptr >= uptr);

  STATIC_CHECK((uptr <=> nullptr) == std::strong_ordering::equal);
  STATIC_CHECK((nullptr <=> uptr) == std::strong_ordering::equal);
}
