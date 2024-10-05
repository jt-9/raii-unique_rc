#include <catch2/catch_test_macros.hpp>


#include "memory_delete.hpp"
#include "unique_rc.hpp"

#include <cstdint>
#include <utility>


TEST_CASE("Default initialised unique_rc<char*, memory_delete<char*>>", "[unique_rc::unique_rc]")
{
  const raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{};

  REQUIRE(char_rc.get() == nullptr);
  REQUIRE_FALSE(char_rc);
  // REQUIRE(factorial(10) == 3628800);
}

TEST_CASE("Move constructor from initialised unique_rc<char*, memory_delete<char*>>", "[unique_rc::unique_rc]")
{
  raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{ new char{ 'A' } };

  CHECK(char_rc.get() != nullptr);
  CHECK(char_rc);

  decltype(char_rc) char_rc2{ std::move(char_rc) };

  REQUIRE(char_rc2);
  REQUIRE(char_rc2.get() != nullptr);
}

TEST_CASE("Converting constructor of unique_rc<std::int8_t*, memory_delete<std::int8_t*>>", "[unique_rc::unique_rc]")
{
  constexpr auto kChar = 'A';
  raii::unique_rc<signed char *, raii::memory_delete<signed char *>> char_rc{ new signed char{ kChar } };

  REQUIRE(char_rc);

  raii::unique_rc<std::int8_t *, raii::memory_delete<std::int8_t *>> int8_rc{ std::move(char_rc) };

  REQUIRE(int8_rc);
  REQUIRE(*int8_rc.get() == kChar);
}

TEST_CASE("Release empty initialised unique_rc<float*, memory_delete<float*>>", "[unique_rc::release()]")
{
  raii::unique_rc<float *, raii::memory_delete<float *>> float_rc{};

  CHECK_FALSE(float_rc);
  CHECK(float_rc.get() == nullptr);

  REQUIRE(float_rc.release() == nullptr);
  REQUIRE_FALSE(float_rc);
}

TEST_CASE("Release initialised unique_rc<float*, memory_delete<float*>>", "[unique_rc::release()]")
{
  constexpr auto test_number = 496;
  raii::unique_rc<float *, raii::memory_delete<float *>> float_rc{ new float{ test_number } };

  CHECK(float_rc);
  CHECK(float_rc.get() != nullptr);

  decltype(float_rc) init_from_release{ float_rc.release() };
  REQUIRE(init_from_release);

  REQUIRE(float_rc.get() == nullptr);
  REQUIRE_FALSE(float_rc);
}

TEST_CASE("Reset initialised unique_rc<float*, memory_delete<float*>>", "[unique_rc::reset()]")
{
  constexpr auto test_number = 496;
  raii::unique_rc<float *, raii::memory_delete<float *>> float_rc{ new float{ test_number } };

  REQUIRE(float_rc);
  REQUIRE(float_rc.get() != nullptr);

  REQUIRE_NOTHROW(float_rc.reset());

  REQUIRE(float_rc.get() == nullptr);
  REQUIRE_FALSE(float_rc);
}

TEST_CASE("Reset empty unique_rc<float*, memory_delete<float*>> default", "[unique_rc::reset()]")
{
  raii::unique_rc<float *, raii::memory_delete<float *>> float_rc{};

  REQUIRE_FALSE(float_rc);
  REQUIRE(float_rc.get() == nullptr);

  REQUIRE_NOTHROW(float_rc.reset());

  REQUIRE(float_rc.get() == nullptr);
  REQUIRE_FALSE(float_rc);
}