#include <catch2/catch_test_macros.hpp>

#include "unique_ptr.hpp"

#include <utility>

namespace {
struct A
{
  A() : b(false) {}

  // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
  A(int /*unused*/, double & /*unused*/, char && /*unused*/, void * /*unused*/) : b(true) {}
  bool b;
};
}// namespace


TEST_CASE("raii::make_unique single object", "[unique_ptr][make_unique]")
{
  const int iparam = 0;
  double dparam = 0;
  char cparam = '0';

  // NOLINTNEXTLINE(hicpp-move-const-arg, performance-move-const-arg)
  raii::unique_ptr ptr = raii::make_unique<A>(iparam, dparam, std::move(cparam), nullptr);
  REQUIRE(ptr != nullptr);
  CHECK(ptr->b);

  ptr = raii::make_unique<A>();
  REQUIRE(ptr != nullptr);
  CHECK_FALSE(ptr->b);
}