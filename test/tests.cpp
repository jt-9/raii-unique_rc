#include <catch2/catch_test_macros.hpp>


#include "memory_delete.hpp"
#include "unique_rc.hpp"

#include <cstdint>
#include <utility>


TEST_CASE("Default initialised unique_rc<char*, memory_delete<char*>>", "[unique_rc]")
{
  const raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{};

  REQUIRE(char_rc.get() == nullptr);
  REQUIRE_FALSE(char_rc);
  // REQUIRE(factorial(10) == 3628800);
}

TEST_CASE("Move constructor from initialised unique_rc<char*, memory_delete<char*>>", "[unique_rc]")
{
  constexpr auto kChar = 'C';
  raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{ new char{ kChar } };

  CHECK(char_rc.get() != nullptr);
  CHECK(char_rc);

  decltype(char_rc) char_rc2{ std::move(char_rc) };

  REQUIRE(char_rc2);
  REQUIRE(char_rc2.get() != nullptr);
  REQUIRE(*char_rc2.get() == kChar);
}

TEST_CASE("Converting constructor of unique_rc<std::int8_t*, memory_delete<std::int8_t*>>", "[unique_rc]")
{
  constexpr auto kChar = 'A';
  raii::unique_rc<signed char *, raii::memory_delete<signed char *>> char_rc{ new signed char{ kChar } };

  REQUIRE(char_rc);

  raii::unique_rc<std::int8_t *, raii::memory_delete<std::int8_t *>> int8_rc{ std::move(char_rc) };

  REQUIRE(int8_rc);
  REQUIRE(*int8_rc.get() == kChar);
}

TEST_CASE("Access handle to get/set value unique_rc<float*, memory_delete<float*>>", "[unique_rc]")
{
  constexpr auto test_number = 28.0F;
  const raii::unique_rc<float *, raii::memory_delete<float *>> rc1{ new float{ test_number } };

  REQUIRE(rc1.get() != nullptr);

  SECTION("Read stored value") { REQUIRE(*rc1.get() == test_number); }

  SECTION("Write new value")
  {
    constexpr auto new_number = 8128.0F;
    REQUIRE_NOTHROW(*rc1.get() = new_number);
    REQUIRE(*rc1.get() == new_number);
  }
}

TEST_CASE("Release empty initialised unique_rc<float*, memory_delete<float*>>", "[unique_rc]")
{
  raii::unique_rc<float *, raii::memory_delete<float *>> default_init_rc{};

  CHECK_FALSE(default_init_rc);
  CHECK(default_init_rc.get() == nullptr);

  REQUIRE(default_init_rc.release() == nullptr);
  REQUIRE_FALSE(default_init_rc);
}

TEST_CASE("Release initialised unique_rc<float*, memory_delete<float*>>", "[unique_rc]")
{
  constexpr auto test_number = 496.0F;
  raii::unique_rc<float *, raii::memory_delete<float *>> rc1{ new float{ test_number } };

  CHECK(rc1);
  CHECK(rc1.get() != nullptr);

  decltype(rc1) init_from_release{ rc1.release() };
  REQUIRE(init_from_release);

  REQUIRE(rc1.get() == nullptr);
  REQUIRE_FALSE(rc1);
}