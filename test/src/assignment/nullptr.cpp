#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

namespace {
struct A
{
};
}// namespace

TEST_CASE("Move assign nullptr to single object unique_ptr", "[unique_ptr][operator=]")
{
  raii::unique_ptr<A> ptr_a(new A);
  CHECK(ptr_a.get() != nullptr);
  ptr_a = nullptr;
  CHECK(ptr_a.get() == nullptr);
}

TEST_CASE("Move assign nullptr to array objects unique_ptr", "[unique_ptr][operator=]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  raii::unique_ptr<A[]> ptr_aa(new A[2]);
  CHECK(ptr_aa.get() != nullptr);
  ptr_aa = nullptr;
  CHECK(ptr_aa.get() == nullptr);
}