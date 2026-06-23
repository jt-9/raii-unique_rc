#include <catch2/catch_test_macros.hpp>

#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

#include <cstddef>// std::size_t
#include <functional>// std::hash


TEST_CASE("std::hash for unique_ptr single value and array of type Empty", "[unique_ptr][hash]")
{
  struct Empty
  {
  };

  const raii::unique_ptr<Empty> ptr0{ new Empty{} };
  const std::hash<raii::unique_ptr<Empty>> hu0;
  const std::hash<raii::unique_ptr<Empty>::pointer> hp0;

  CHECK(hu0(ptr0) == hp0(ptr0.get()));

  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  const raii::unique_ptr<Empty[]> ptr1{ new Empty[10] };
  const std::hash<raii::unique_ptr<Empty[]>> hu1;
  const std::hash<raii::unique_ptr<Empty[]>::pointer> hp1;
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)

  CHECK(hu1(ptr1) == hp1(ptr1.get()));
}


namespace {
struct D
{
  struct pointer
  {
    // cppcheck-suppress noExplicitConstructor
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr pointer(std::nullptr_t = nullptr) {};

    bool operator==(std::nullptr_t) const { return false; }
    bool operator!=(std::nullptr_t) const { return true; }
  };

  void operator()(pointer /*unused*/) const noexcept {}
};

struct F
{
  struct pointer
  {
    // cppcheck-suppress noExplicitConstructor
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr pointer(std::nullptr_t = nullptr) {};

    bool operator==(std::nullptr_t) const { return false; }
    bool operator!=(std::nullptr_t) const { return true; }
  };

  void operator()(pointer /*unused*/) const noexcept {}
};

}// namespace

namespace std {
template<> struct hash<D::pointer>
{
  // NOLINTNEXTLINE(hicpp-exception-baseclass)
  std::size_t operator()(D::pointer /*unused*/) const { throw 1; }
};

template<> struct hash<F::pointer>
{
  // NOLINTNEXTLINE(hicpp-exception-baseclass)
  std::size_t operator()(F::pointer /*unused*/) const { throw 3; }
};
}// namespace std

TEST_CASE("hash::operator() throws with Deleter::pointer type", "[unique_ptr][hash][throw]")
{
  using UP = raii::unique_ptr<int, D>;
  const UP ptr1;
  const std::hash<UP> hu1;

  REQUIRE_THROWS_AS(hu1(ptr1), int);

  // Should still be noexcept if the underlying hash object is:
  using UP2 = raii::unique_ptr<int>;
  constexpr UP2 ptr2;
  constexpr std::hash<UP2> hu2;

  STATIC_CHECK(noexcept(hu2(ptr2)));


  using UP3 = raii::unique_ptr<int, F>;
  const UP3 ptr3;
  const std::hash<UP3> hu3;

  REQUIRE_THROWS_AS(hu3(ptr3), int);
}
