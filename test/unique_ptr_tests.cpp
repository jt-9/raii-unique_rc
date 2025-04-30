#include <catch2/catch_test_macros.hpp>

#include "unique_ptr.hpp"

#include <algorithm>
#include <array>
#include <cstddef>// std::size_t
#include <utility>


TEST_CASE("Default initialised unique_ptr<char>", "[unique_ptr]")
{
  constexpr raii::unique_ptr<char> ptr1{};

  REQUIRE(ptr1.get() == nullptr);
  REQUIRE_FALSE(ptr1);
}

TEST_CASE("Move constructor from initialised unique_ptr<char>", "[unique_ptr]")
{
  constexpr auto kChar = 'C';
  raii::unique_ptr<char> ptr1{ new char{ kChar } };

  CHECK(ptr1.get() != nullptr);
  CHECK(ptr1);

  decltype(ptr1) ptr2{ std::move(ptr1) };

  REQUIRE(ptr2);
  REQUIRE(ptr2.get() != nullptr);
  REQUIRE(*ptr2.get() == kChar);
}

TEST_CASE("Indirection operator* to get/set value unique_ptr<float>", "[unique_ptr]")
{
  constexpr auto test_number = 28.0F;
  raii::unique_ptr<float> ptr1{ new float{ test_number } };

  REQUIRE(ptr1.get() != nullptr);

  SECTION("Read stored value")
  {
    REQUIRE(*ptr1 == test_number);
    REQUIRE(*ptr1 == *ptr1.get());
  }

  SECTION("Write new value")
  {
    constexpr auto new_number = 8128.0F;
    REQUIRE_NOTHROW(*ptr1 = new_number);
    REQUIRE(*ptr1 == new_number);
    REQUIRE(*ptr1 == *ptr1.get());
  }
}

TEST_CASE("Release empty initialised unique_ptr<float>", "[unique_ptr]")
{
  raii::unique_ptr<float> default_init{};

  CHECK_FALSE(default_init);
  CHECK(default_init.get() == nullptr);

  REQUIRE(default_init.release() == nullptr);
  REQUIRE_FALSE(default_init);
}

TEST_CASE("Release initialised unique_ptr<float>", "[unique_ptr]")
{
  constexpr auto test_number = 496.0F;
  raii::unique_ptr<float> ptr1{ new float{ test_number } };

  CHECK(ptr1);
  CHECK(ptr1.get() != nullptr);

  decltype(ptr1) init_from_release{ ptr1.release() };
  REQUIRE(init_from_release);

  REQUIRE(ptr1.get() == nullptr);
  REQUIRE_FALSE(ptr1);
}


TEST_CASE("Reset empty unique_ptr<float> default", "[unique_ptr]")
{
  raii::unique_ptr<float> ptr1{};

  REQUIRE_FALSE(ptr1);
  REQUIRE(ptr1.get() == nullptr);

  REQUIRE_NOTHROW(ptr1.reset());

  REQUIRE(ptr1.get() == nullptr);
  REQUIRE_FALSE(ptr1);
}

TEST_CASE("Default initialised unique_ptr<char[]>", "[unique_ptr]")
{
  // NOLINTNEXTLINE cppcoreguidelines-avoid-c-arrays and similar
  constexpr raii::unique_ptr<char[]> ptr1{};

  REQUIRE(ptr1.get() == nullptr);
  REQUIRE_FALSE(ptr1);
}

TEST_CASE("Move constructor from initialised unique_ptr<char[]>", "[unique_ptr]")
{
  constexpr std::array input = { 'A', 'B', 'C', 'D', 'y', 'w' };

  using elem_type = decltype(input)::value_type;
  // NOLINTNEXTLINE cppcoreguidelines-avoid-c-arrays and similar
  raii::unique_ptr<elem_type[]> ptr1 = raii::make_unique_for_overwrite<elem_type[]>(input.size());

  std::ranges::copy(input, ptr1.get());

  CHECK(ptr1.get() != nullptr);
  CHECK(ptr1);

  decltype(ptr1) ptr2{ std::move(ptr1) };

  REQUIRE(ptr2);
  REQUIRE(ptr2.get() != nullptr);

  REQUIRE(std::equal(std::cbegin(input), std::cend(input), ptr2.get()));
}

TEST_CASE("Array subscript operator unique_ptr<int[]>", "[unique_ptr]")
{
  constexpr std::array input = { 1, 3, 5, 7, 11, 13, 17, 19 };

  using elem_type = decltype(input)::value_type;
  // NOLINTNEXTLINE cppcoreguidelines-avoid-c-arrays and similar
  raii::unique_ptr<elem_type[]> ptr1 = raii::make_unique_for_overwrite<elem_type[]>(input.size());

  CHECK(ptr1.get() != nullptr);
  CHECK(ptr1);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  for (std::size_t i = 0; i < input.size(); i++) { ptr1[i] = input[i]; }

  REQUIRE(std::equal(std::cbegin(input), std::cend(input), ptr1.get()));
}