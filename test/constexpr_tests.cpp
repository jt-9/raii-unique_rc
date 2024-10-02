#include <catch2/catch_test_macros.hpp>

#include "memory_deleter.hpp"
#include "unique_rc.hpp"

TEST_CASE("Default initialised unique_rc<char*, memory_delete<char*>>", "[unique_rc::unique_rc()]")
{
  constexpr raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{};

  STATIC_REQUIRE(char_rc.get() == nullptr);
  STATIC_REQUIRE_FALSE(char_rc);
}

TEST_CASE("invalid() of default initialised unique_rc<char*, memory_delete<char*>>", "[unique_rc::invalid()]")
{
  constexpr raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{};

  STATIC_REQUIRE(decltype(char_rc)::invalid() == nullptr);
}

TEST_CASE("Equality between default initialised unique_rc<int*, memory_delete<int*>>",
  "[unique_rc::operator ==(unique_rc)]")
{
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc1{};
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc2{};

  STATIC_REQUIRE(int_rc1 == int_rc2);
}

TEST_CASE("Three-way comparison between default initialised unique_rc<int*, memory_delete<int*>>",
  "[unique_rc::operator <=>(unique_rc)]")
{
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc1{};
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc2{};

  STATIC_REQUIRE((int_rc1 <=> int_rc2) == std::strong_ordering::equal);
}