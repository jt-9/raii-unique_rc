#include <catch2/catch_test_macros.hpp>

#include "urc/unique_ptr.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>// std::size_t
#include <span>


namespace {
template<typename T>
[[nodiscard]] constexpr bool unique_ptr_value_array_subscript(std::size_t array_size, T first_element) noexcept
{
  // NOLINTNEXTLINE
  auto ptr = raii::unique_ptr<T[]>{ new T[array_size]{} };
  assert(ptr.get() != nullptr);

  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
  ptr[0] = first_element;
  assert(ptr[0] == first_element);

  assert(ptr[array_size - 1] == T{});
  // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)

  return true;
}
}// namespace

TEST_CASE("unique_ptr array of ints, unique_ptr::operator[]", "[unique_ptr][subscript operator][constexpr]")
{ STATIC_CHECK(unique_ptr_value_array_subscript(5, 42)); }

TEST_CASE("Array subscript operator unique_ptr<int[]>", "[unique_ptr][subscript operator]")
{
  constexpr std::array input = { 1, 3, 5, 7, 11, 13, 17, 19 };

  using elem_type = decltype(input)::value_type;
  // NOLINTNEXTLINE cppcoreguidelines-avoid-c-arrays and similar
  raii::unique_ptr<elem_type[]> ptr1 = raii::make_unique_for_overwrite<elem_type[]>(input.size());

  CHECK(ptr1.get() != nullptr);
  CHECK(ptr1);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,
  // cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
  for (std::size_t i = 0; i < input.size(); i++) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access, cppcoreguidelines-pro-bounds-constant-array-index)
    REQUIRE_NOTHROW(ptr1[i] = input[i]);
  }

  CHECK(std::ranges::equal(input, std::span{ ptr1.get(), input.size() }));
}