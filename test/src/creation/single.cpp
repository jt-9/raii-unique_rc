#include <catch2/catch_test_macros.hpp>

#include "urc/unique_ptr.hpp"

#include <cstddef>// std::size_t
#include <initializer_list>
#include <utility>

namespace {
struct A
{
  A() : b(false) {}

  // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
  A(int /*unused*/, double & /*unused*/, char && /*unused*/, void * /*unused*/) : b(true) {}
  bool b;
};

template<typename T> struct MultiInit
{
  constexpr MultiInit(std::size_t /*unused*/, const T & /*unused*/) noexcept : i{ 1 } {}

  constexpr MultiInit(std::initializer_list<T> /*unused*/) noexcept : i{ 2 } {}

  int i;
};
}// namespace


TEST_CASE("raii::make_unique single object", "[unique_ptr][make_unique]")
{
  const int iparam = 0;
  double dparam = 0;
  char cparam = '0';

  // NOLINTNEXTLINE(hicpp-move-const-arg, performance-move-const-arg)
  raii::unique_ptr ptr = raii::make_unique<A>(iparam, dparam, std::move(cparam), nullptr);
  CHECK(ptr != nullptr);
  CHECK(ptr->b);

  ptr = raii::make_unique<A>();
  CHECK(ptr != nullptr);
  CHECK_FALSE(ptr->b);
}


TEST_CASE("raii::make_unique direct and list initialisation with make_unique", "[unique_ptr][make_unique]")
{
  const raii::unique_ptr ptr_direct = raii::make_unique<MultiInit<int>>(1ZU, 2);
  CHECK(ptr_direct != nullptr);
  REQUIRE(ptr_direct->i == 1);

  const raii::unique_ptr ptr_direct_list = raii::make_unique<MultiInit<int>>(std::initializer_list{ 1, 2 });
  CHECK(ptr_direct_list != nullptr);
  REQUIRE(ptr_direct_list->i == 2);
}