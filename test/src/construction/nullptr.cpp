#include <catch2/catch_test_macros.hpp>

#include "memory_delete.hpp"
#include "unique_ptr.hpp"
#include "unique_rc.hpp"

namespace {
struct A
{
};
}// namespace


TEST_CASE("constexpr default initialised unique_rc single object A*", "[unique_rc][unique_rc::unique_rc]")
{
  using URc = raii::unique_rc<A *, raii::memory_delete<A *>>;

  constexpr URc urc{};

  STATIC_REQUIRE(urc.get() == nullptr);
  STATIC_REQUIRE_FALSE(urc);
  STATIC_REQUIRE(urc == nullptr);

  STATIC_REQUIRE(decltype(urc)::invalid() == nullptr);
}

TEST_CASE("constexpr default initialised unique_ptr single object", "[unique_ptr][unique_ptr::unique_ptr]")
{
  using UPtr = raii::unique_ptr<A>;

  constexpr UPtr uptr{};

  STATIC_REQUIRE(uptr.get() == nullptr);
  STATIC_REQUIRE_FALSE(uptr);
  STATIC_REQUIRE(uptr == nullptr);

  STATIC_REQUIRE(decltype(uptr)::invalid() == nullptr);
}

TEST_CASE("constexpr default initialised unique_ptr array of objects", "[unique_ptr][unique_ptr::unique_ptr]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  using UPtr = raii::unique_ptr<A[]>;

  constexpr UPtr uptr{};

  STATIC_REQUIRE(uptr.get() == nullptr);
  STATIC_REQUIRE_FALSE(uptr);
  STATIC_REQUIRE(uptr == nullptr);

  STATIC_REQUIRE(decltype(uptr)::invalid() == nullptr);
}