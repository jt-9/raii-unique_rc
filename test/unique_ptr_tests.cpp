#include <catch2/catch_test_macros.hpp>


#include "mock_pointer_no_op.hpp"
#include "unique_ptr.hpp"

#include <algorithm>
#include <array>
#include <cstddef>// std::size_t
#include <cstdint>// std::int8_t
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

TEST_CASE("Converting constructor of unique_ptr<std::int8_t>", "[unique_ptr]")
{
  constexpr auto kChar = 'A';
  raii::unique_ptr<signed char> ptr1{ new signed char{ kChar } };

  REQUIRE(ptr1);

  raii::unique_ptr<std::int8_t> ptr2{ std::move(ptr1) };

  REQUIRE(ptr2);
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

TEST_CASE("Reset initialised unique_ptr<float>", "[unique_ptr]")
{
  constexpr auto test_number = 8128.0F;
  raii::unique_ptr<float> ptr1{ new float{ test_number } };

  REQUIRE(ptr1);
  REQUIRE(ptr1.get() != nullptr);
  CHECK(*ptr1 == test_number);

  SECTION("Reset with invalid value (nullptr)")
  {
    REQUIRE_NOTHROW(ptr1.reset());

    REQUIRE(ptr1.get() == nullptr);
    REQUIRE_FALSE(ptr1);
  }

  SECTION("Reset with other constructed float")
  {
    constexpr auto test_number2 = -6.0F;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto *const non_owner_ptr = new float{ test_number2 };
    ptr1.reset(non_owner_ptr);

    REQUIRE(ptr1.get() == non_owner_ptr);
    REQUIRE(ptr1);
  }
}

TEST_CASE("Equality of value initialised unique_ptr<double>", "[unique_ptr]")
{
  const auto test_number = 13.11;
  const raii::unique_ptr<double> ptr1{ new double{ test_number } };

  CHECK(ptr1);

  SECTION("unique_ptr::operator == to other value constructed double*")
  {
    const raii::unique_ptr<double> ptr2{ new double{ 1 - test_number } };
    CHECK(ptr2);

    REQUIRE_FALSE(ptr1 == ptr2);
    REQUIRE(ptr1 != ptr2);
  }

  SECTION("unique_ptr::operator == to same, but non-owning unique_ptr")
  {
    const raii::unique_ptr<double, mock_raii::mock_pointer_no_op<double *>> noop_ptr{ ptr1.get() };

    REQUIRE(ptr1 == noop_ptr);
    REQUIRE_FALSE(ptr1 != noop_ptr);
  }

  SECTION("unique_ptr::operator == to default constructed unique_ptr")
  {
    constexpr raii::unique_ptr<double> default_initalised{};
    CHECK_FALSE(default_initalised);

    REQUIRE(ptr1 != default_initalised);
    REQUIRE(ptr1 != nullptr);

    REQUIRE(default_initalised != ptr1);
    REQUIRE(nullptr != ptr1);
  }
}

TEST_CASE("Three-way operator <=> with value initialised unique_ptr<int>", "[unique_ptr]")
{
  const raii::unique_ptr<int> ptr1{ new int{ 73 } };

  CHECK(ptr1);

  SECTION("With other value constructed unique_ptr")
  {
    const raii::unique_ptr<int> ptr2{ new int{ 37 } };
    CHECK(ptr2);

    REQUIRE((ptr1 <=> ptr2) != std::strong_ordering::equal);
  }

  SECTION("With same, but non-owning unique_ptr")
  {
    const raii::unique_ptr<int, mock_raii::mock_pointer_no_op<int *>> noop_ptr{ ptr1.get() };

    REQUIRE((ptr1 <=> noop_ptr) == std::strong_ordering::equal);

    const auto equals_to_zero = (ptr1 <=> noop_ptr) == 0;
    REQUIRE(equals_to_zero);
  }

  SECTION("Default constructed unique_ptr")
  {
    constexpr raii::unique_ptr<int> default_init{};
    CHECK_FALSE(default_init);

    REQUIRE((ptr1 <=> default_init) == std::strong_ordering::greater);
    REQUIRE((ptr1 <=> nullptr) == std::strong_ordering::greater);

    REQUIRE((default_init <=> ptr1) == std::strong_ordering::less);
    REQUIRE((nullptr <=> ptr1) == std::strong_ordering::less);
  }
}

TEST_CASE("Swap value initialised unique_ptr<int>", "[unique_ptr]")
{
  const auto test_number = 289;
  raii::unique_ptr<int> ptr1 = raii::make_unique<int>(test_number);

  CHECK(ptr1);

  SECTION("unique_ptr::swap with other value constructed unique_ptr")
  {
    const auto test_number2 = -36;
    using element_type = decltype(ptr1)::element_type;
    raii::unique_ptr<element_type, decltype(ptr1)::deleter_type> ptr2{ new int{ test_number2 } };
    REQUIRE(ptr2);

    auto *const raw_ptr1 = ptr1.get();
    auto *const raw_ptr2 = ptr2.get();

    ptr1.swap(ptr2);

    CHECK(ptr1);
    CHECK(ptr2);

    REQUIRE(raw_ptr2 == ptr1.get());
    REQUIRE(raw_ptr1 == ptr2.get());
  }

  SECTION("unique_ptr::swap with default constructed unique_ptr")
  {
    using element_type = decltype(ptr1)::element_type;
    raii::unique_ptr<element_type, decltype(ptr1)::deleter_type> ptr2{};
    CHECK_FALSE(ptr2);

    auto *const raw_ptr1 = ptr1.get();
    auto *const raw_ptr2 = ptr2.get();

    ptr1.swap(ptr2);

    REQUIRE(raw_ptr2 == ptr1.get());
    REQUIRE(raw_ptr1 == ptr2.get());
  }
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