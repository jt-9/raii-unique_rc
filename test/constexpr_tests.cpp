#include <catch2/catch_test_macros.hpp>

#include "unique_rc.hpp"
#include "memory_deleter.hpp"

TEST_CASE("Default initialised unique_rc<char*, memory_delete<char*>> default", "[unique_rc]")
{
  constexpr raii::unique_rc<char*, raii::memory_delete<char*>> default_init_rc{};

  STATIC_REQUIRE(default_init_rc.get() == nullptr);
  STATIC_REQUIRE_FALSE(default_init_rc);
  // STATIC_REQUIRE(factorial_constexpr(10) == 3628800);
}
