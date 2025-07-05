#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"

#include <type_traits>


namespace {
struct A
{
  void operator()(void * /*unused*/) const {}
};

struct B
{
  using pointer = char *;
  void operator()(pointer /*unused*/) const {}
};
}// namespace

TEST_CASE("Array objects unique_ptr::pointer type deduction", "[unique_ptr][unique_ptr::pointer]")
{
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  using up = raii::unique_ptr<int[]>;
  using upA = raii::unique_ptr<int[], A>;
  using upB = raii::unique_ptr<int[], B>;
  using upAr = raii::unique_ptr<int[], A &>;
  using upBr = raii::unique_ptr<int[], B &>;
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)

  STATIC_CHECK(std::is_same_v<up::pointer, int *>);
  STATIC_CHECK(std::is_same_v<upA::pointer, int *>);
  STATIC_CHECK(std::is_same_v<upB::pointer, char *>);
  STATIC_CHECK(std::is_same_v<upAr::pointer, int *>);
  STATIC_CHECK(std::is_same_v<upBr::pointer, char *>);
}