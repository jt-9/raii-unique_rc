#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

#include <array>
#include <cassert>
#include <cstddef>


namespace {

template<typename T> constexpr bool unique_ptr_value_single(T const val1, T const val2) noexcept
{
  raii::unique_ptr<T> ptr1;
  ptr1.reset();
  assert(!ptr1);
  // Catch 3.8.1 doesn't work with constexpr: error non-constexpr constructor 'AssertionHandler' cannot be used in a
  // constant expression CHECK_FALSE(ptr1);

  ptr1.reset(nullptr);
  assert(!ptr1);
  // CHECK(!ptr1);

  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, cppcoreguidelines-owning-memory)
  ptr1.reset(new T{ val1 });
  assert(ptr1);
  assert(val1 == *ptr1);
  // CHECK(*ptr1 == 2);

  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, cppcoreguidelines-owning-memory)
  ptr1.reset(new T{ val2 });
  assert(val2 == *ptr1);
  // CHECK(*ptr1 == 3);
  ptr1.reset(nullptr);
  assert(!ptr1);
  // CHECK(!ptr1);

  return true;
}

// template<typename T>
constexpr bool unique_ptr_value_array(int const elem1, int const elem2, int const elem3, int const elem4) noexcept
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  raii::unique_ptr<int[]> ptra1;
  ptra1.reset();
  assert(!ptra1);

  ptra1.reset(nullptr);
  assert(!ptra1);

  // cppcheck-suppress leakNoVarFunctionCall false positive
  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, cppcoreguidelines-owning-memory)
  ptra1.reset(new int[]{ elem1, elem2 });
  assert(elem1 == ptra1[0]);

  // cppcheck-suppress leakNoVarFunctionCall false positive
  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, cppcoreguidelines-owning-memory)
  ptra1.reset(new int[]{ elem1, elem2, elem3, elem4 });
  assert(elem2 == ptra1[1]);
  assert(elem4 == ptra1[3]);

  ptra1.reset(nullptr);
  assert(!ptra1);

  return true;
}

template<typename T> constexpr bool unique_ptr_default_array(std::size_t array_size) noexcept
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  raii::unique_ptr<const T[]> ptra2;

  // cppcheck-suppress leakNoVarFunctionCall false positive
  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, cppcoreguidelines-owning-memory)
  ptra2.reset(new T[array_size]{});

  constexpr auto default_value = T{};
  // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
  for (std::size_t i = 0; i < array_size; i++) { assert(default_value == ptra2[i]); }

  ptra2.reset(nullptr);
  assert(!ptra2);

  return true;
}

template<typename T> constexpr bool unique_ptr_default_array_reset_nullptr(std::size_t array_size) noexcept
{
  // NOLINTNEXTLINE
  raii::unique_ptr<T[]> ptra1{ new T[array_size] };
  assert(ptra1);

  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new)
  ptra1.reset(new T[array_size + 3]);
  assert(ptra1);

  ptra1.reset(nullptr);
  assert(!ptra1);

  ptra1.reset();
  assert(!ptra1);

  return true;
}

}// namespace

TEST_CASE("constexpr unique_ptr::reset", "[unique_ptr][reset]")
{
  STATIC_CHECK(unique_ptr_value_single(4, -2));
  STATIC_CHECK(unique_ptr_value_array('a', 'B', 'C', 'd'));
  STATIC_CHECK(unique_ptr_default_array<int>(7));
  STATIC_CHECK(unique_ptr_default_array_reset_nullptr<char>(3));
  STATIC_CHECK(unique_ptr_default_array_reset_nullptr<std::array<int, 4>>(3));
}