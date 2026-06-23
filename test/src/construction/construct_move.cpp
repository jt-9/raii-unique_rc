#include <catch2/catch_test_macros.hpp>

#include "urc/memory_delete.hpp"
#include "urc/unique_ptr.hpp"
#include "urc/unique_rc.hpp"

#include <algorithm>// std::ranges::copy
#include <array>
#include <cassert>
#include <span>
#include <string>
#include <utility>


namespace {
template<typename T> constexpr bool ConstructURcSingle(const T &initValue) noexcept
{
  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new)
  raii::unique_rc<T *, raii::memory_delete<T *>> urc1{ new T{ initValue } };

  assert(urc1.get() != nullptr);
  assert(urc1);

  decltype(urc1) const urc2{ std::move(urc1) };

  assert(urc2);
  assert(urc2.get() != nullptr);
  assert(*urc2.get() == initValue);

  return true;
}

template<typename T> constexpr bool ConstructUPtrSingle(const T &initValue) noexcept
{
  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new)
  raii::unique_ptr<T> ptr1{ new T{ initValue } };

  assert(ptr1.get() != nullptr);
  assert(ptr1);

  decltype(ptr1) const ptr2{ std::move(ptr1) };

  assert(ptr2);
  assert(ptr2.get() != nullptr);
  assert(*ptr2.get() == initValue);

  return true;
}
}// namespace

TEST_CASE("unique_rc move-constructed from initialised unique_rc<char*, memory_delete<char*>>",
  "[unique_rc][unique_rc::unique_rc]")
{
  STATIC_CHECK(ConstructURcSingle('Q'));
  STATIC_CHECK(ConstructURcSingle(std::string{ "constexpr urc" }));
}

TEST_CASE("unique_ptr move-constructored from initialised unique_ptr<char>", "[unique_ptr][unique_ptr::unique_ptr]")
{ STATIC_CHECK(ConstructUPtrSingle('C')); }

TEST_CASE("move-constructed unique_ptr<char[]> from std::array", "[unique_ptr][unique_ptr::unique_ptr]")
{
  constexpr std::array input = { 'A', 'B', 'C', 'D', 'y', 'w' };

  using elem_type = decltype(input)::value_type;
  // NOLINTNEXTLINE cppcoreguidelines-avoid-c-arrays and similar
  raii::unique_ptr<elem_type[]> ptr1 = raii::make_unique_for_overwrite<elem_type[]>(input.size());

  std::ranges::copy(input, ptr1.get());

  CHECK(ptr1.get() != nullptr);
  CHECK(ptr1);

  decltype(ptr1) ptr2{ std::move(ptr1) };

  CHECK(ptr2);
  CHECK(ptr2.get() != nullptr);

  CHECK(std::ranges::equal(input, std::span{ ptr2.get(), input.size() }));
}