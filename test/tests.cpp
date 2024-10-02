#include <catch2/catch_test_macros.hpp>


#include "memory_deleter.hpp"
#include "unique_rc.hpp"


TEST_CASE("Default initialised unique_rc<char*, memory_delete<char*>> default", "[unique_rc::unique_rc()]")
{
  const raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{};

  REQUIRE(char_rc.get() == nullptr);
  REQUIRE_FALSE(char_rc);
  // REQUIRE(factorial(10) == 3628800);
}

TEST_CASE("Default initialised unique_rc<char*, memory_delete<char*>> default", "[unique_rc::release()]")
{
  raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{};

  REQUIRE(char_rc.release() == nullptr);
}

TEST_CASE("Initialised unique_rc<char*, memory_delete<char*>> default", "[unique_rc::unique_rc(new char)]")
{
  raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{ new char{ 'A' } };

  CHECK(char_rc.get() != nullptr);
  CHECK(char_rc);
  CHECK_NOTHROW(char_rc.reset());
  CHECK_FALSE(char_rc);
  REQUIRE(char_rc.release() == nullptr);
}