#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

#include <cassert>
// #include <cstddef>


namespace {
template<typename T> [[nodiscard]] constexpr bool unique_ptr_default_single() noexcept
{
  raii::unique_ptr<T> ptr1;
  T *raw_ptr = ptr1.release();
  assert(raw_ptr == nullptr);
  assert(!ptr1);

  return true;
}

template<typename T> [[nodiscard]] constexpr bool unique_ptr_value_single(T const val) noexcept
{
  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new)
  raii::unique_ptr<T> ptr2(new T{ val });
  auto *const raw_ptr = ptr2.release();
  assert(raw_ptr);
  assert(!ptr2);

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete raw_ptr;

  return true;
}

template<typename T> [[nodiscard]] constexpr bool unique_ptr_default_array() noexcept
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  raii::unique_ptr<T[]> ptr1;
  auto *raw_ptr = ptr1.release();
  assert(raw_ptr == nullptr);
  assert(!ptr1);

  return true;
}

[[nodiscard]] constexpr bool unique_ptr_value_array(int const elem1, int const elem2, int const elem3) noexcept
{
  // NOLINTNEXTLINE
  raii::unique_ptr<int[]> ptr2{ new int[]{ elem1, elem2, elem3 } };
  auto *const raw_ptr = ptr2.release();
  assert(raw_ptr);
  assert(!ptr2);

  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  assert(elem1 == raw_ptr[0]);
  assert(elem2 == raw_ptr[1]);
  // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] raw_ptr;

  return true;
}
}// namespace


TEST_CASE("constexpr unique_ptr::release", "[unique_ptr][release]")
{
  STATIC_CHECK(unique_ptr_default_single<int>());
  STATIC_CHECK(unique_ptr_value_single('B'));
  STATIC_CHECK(unique_ptr_default_array<float>());
  STATIC_CHECK(unique_ptr_value_array(7, -3, 11));
}