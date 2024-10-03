#include <catch2/catch_test_macros.hpp>

#include "memory_deleter.hpp"
#include "unique_rc.hpp"


TEST_CASE("Default initialised unique_rc<char*, memory_delete<char*>>", "[unique_rc]")
{
  constexpr raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{};

  SECTION("unique_rc::unique_rc()")
  {
    STATIC_REQUIRE(char_rc.get() == nullptr);
    STATIC_REQUIRE_FALSE(char_rc);
  }

  SECTION("unique_rc::invalid()") { STATIC_REQUIRE(decltype(char_rc)::invalid() == nullptr); }
}

TEST_CASE("Equality of default initialised unique_rc<int*, memory_delete<int*>>", "[unique_rc::operator ==]")
{
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc1{};
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc2{};

  SECTION("unique_rc::operator ==(unique_rc, unique_rc)")
  {
    STATIC_CHECK_FALSE(int_rc1);
    STATIC_CHECK_FALSE(int_rc2);
    STATIC_REQUIRE(int_rc1 == int_rc2);
  }

  SECTION("unique_rc::operator ==(unique_rc, std::nullptr_t)") { STATIC_REQUIRE(int_rc1 == nullptr); }

  SECTION("unique_rc::operator ==(std::nullptr_t, unique_rc)") { STATIC_REQUIRE(nullptr == int_rc2); }
}

TEST_CASE("Three-way comparison of default initialised unique_rc<int*, memory_delete<int*>>",
  "[unique_rc::operator <=>]")
{
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc1{};
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc2{};

  SECTION("unique_rc::operator <=>(unique_rc, unique_rc)")
  {
    STATIC_CHECK_FALSE(int_rc1);
    STATIC_CHECK_FALSE(int_rc2);
    STATIC_REQUIRE((int_rc1 <=> int_rc2) == std::strong_ordering::equal);
  }

  SECTION("unique_rc::operator <=>(unique_rc, std::nullptr_t)")
  {
    STATIC_REQUIRE((int_rc1 <=> nullptr) == std::strong_ordering::equal);
  }

  SECTION("unique_rc::operator <=>(std::nullptr_t, unique_rc)")
  {
    STATIC_REQUIRE((nullptr <=> int_rc2) == std::strong_ordering::equal);
  }
}