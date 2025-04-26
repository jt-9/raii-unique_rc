#include <catch2/catch_test_macros.hpp>


#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

#include <utility>

namespace {
struct B
{
  B() = default;
  B(const B &) = default;
  B(B &&) = default;

  B &operator=(const B &) = default;
  B &operator=(B &&) = default;

  virtual ~B() = default;
};

struct D : public B
{
};
}// namespace


TEST_CASE("Move assign array objects unique_ptr to default initialised unique_ptr", "[unique_ptr][operator=]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  D *const raw_d = new D[3];

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  raii::unique_ptr<D[]> ptr1(raw_d);
  REQUIRE(ptr1.get() == raw_d);
  REQUIRE(ptr1);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  raii::unique_ptr<D[]> ptr2;
  REQUIRE(ptr2.get() == nullptr);
  REQUIRE_FALSE(ptr2);

  ptr2 = std::move(ptr1);

  //  REQUIRE_FALSE(ptr1); Access of moved variable
  REQUIRE(ptr2.get() == raw_d);
  REQUIRE(ptr2);
}