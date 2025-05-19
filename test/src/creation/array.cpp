#include <catch2/catch_test_macros.hpp>

#include "unique_ptr.hpp"

#include <cstddef>

namespace {
struct A
{
  A() : b(true) {}
  explicit A(int /*unused*/) : b(false) {}
  bool b;
};

constexpr std::size_t constArraySize = 3;
}// namespace


TEST_CASE("raii::make_unique array of objects", "[unique_ptr][make_unique]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  raii::unique_ptr aptr = raii::make_unique<A[]>(constArraySize);
  CHECK(aptr != nullptr);

  for (std::size_t i = 0; i < constArraySize; i++) { CHECK(aptr[i].b); }
}