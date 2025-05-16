#include <catch2/catch_test_macros.hpp>


#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

#include <cassert>
// #include <cstddef>


namespace {
template<typename T> constexpr bool ConstexprUPtrDefaultSingle() noexcept
{
  raii::unique_ptr<T> ptr1;
  T *raw_ptr = ptr1.release();
  assert(raw_ptr == nullptr);
  assert(!ptr1);

  return true;
}

template<typename T> constexpr bool ConstexprUPtrValueSingle(T const val) noexcept
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

template<typename T> constexpr bool ConstexprUPtrDefaultArray() noexcept
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  raii::unique_ptr<T[]> ptr1;
  auto *raw_ptr = ptr1.release();
  assert(raw_ptr == nullptr);
  assert(!ptr1);

  return true;
}

constexpr bool ConstexprUPtrValueArray(int const elem1, int const elem2, int const elem3) noexcept
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
  STATIC_REQUIRE(ConstexprUPtrDefaultSingle<int>());
  STATIC_REQUIRE(ConstexprUPtrValueSingle('B'));
  STATIC_REQUIRE(ConstexprUPtrDefaultArray<float>());
  STATIC_REQUIRE(ConstexprUPtrValueArray(7, -3, 11));
}