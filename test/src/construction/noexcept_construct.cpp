#include <catch2/catch_test_macros.hpp>


#include "memory_delete.hpp"
#include "unique_ptr.hpp"
#include "unique_rc.hpp"

#include <type_traits>


TEST_CASE("unique_ptr must be noexcept move constructible", "[unique_ptr][unique_ptr::unique_ptr][noexcept]")
{
  using iuptr = raii::unique_ptr<int>;
  STATIC_REQUIRE(std::is_nothrow_move_constructible_v<iuptr>);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  using iauptr = raii::unique_ptr<int[]>;
  STATIC_REQUIRE(std::is_nothrow_move_constructible_v<iauptr>);
}

TEST_CASE("unique_rc must be noexcept move constructible", "[unique_rc][unique_rc::unique_rc][noexcept]")
{
  using lurc = raii::unique_rc<long*, raii::memory_delete<long*>>;
  STATIC_REQUIRE(std::is_nothrow_move_constructible_v<lurc>);
}