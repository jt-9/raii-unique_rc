#include <catch2/catch_test_macros.hpp>

#include "urc/memory_delete.hpp"
#include "urc/unique_ptr.hpp"
#include "urc/unique_rc.hpp"


namespace {
struct A
{
};
}// namespace


TEST_CASE("constexpr default initialised unique_rc single object A*", "[unique_rc][unique_rc::unique_rc]")
{
  using URc = raii::unique_rc<A *, raii::memory_delete<A *>>;

  constexpr URc urc{};

  STATIC_CHECK(urc.get() == nullptr);
  STATIC_CHECK_FALSE(urc);
  STATIC_CHECK(urc == nullptr);

  STATIC_CHECK(decltype(urc)::invalid() == nullptr);
}

TEST_CASE("constexpr default initialised unique_ptr single object", "[unique_ptr][unique_ptr::unique_ptr]")
{
  using UPtr = raii::unique_ptr<A>;

  constexpr UPtr uptr{};

  STATIC_CHECK(uptr.get() == nullptr);
  STATIC_CHECK_FALSE(uptr);
  STATIC_CHECK(uptr == nullptr);

  STATIC_CHECK(decltype(uptr)::invalid() == nullptr);
}

TEST_CASE("constexpr default initialised unique_ptr array of objects", "[unique_ptr][unique_ptr::unique_ptr]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  using UPtr = raii::unique_ptr<A[]>;

  constexpr UPtr uptr{};

  STATIC_CHECK(uptr.get() == nullptr);
  STATIC_CHECK_FALSE(uptr);
  STATIC_CHECK(uptr == nullptr);

  STATIC_CHECK(decltype(uptr)::invalid() == nullptr);
}