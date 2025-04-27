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


TEST_CASE("Move assign single object unique_ptr to other unique_ptr", "[unique_ptr][operator=]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  D *const raw_d = new D{};

  raii::unique_ptr<D> ptr1(raw_d);
  REQUIRE(ptr1.get() == raw_d);
  REQUIRE(ptr1);

  raii::unique_ptr<D> ptr2{new D{}};
  REQUIRE(ptr2);

  ptr2 = std::move(ptr1);

  // cppcheck-suppress accessMoved only in test to make sure the pointer is nullptr
  REQUIRE_FALSE(ptr1);
  REQUIRE(ptr2.get() == raw_d);
  REQUIRE(ptr2);

  raii::unique_ptr<B> ptr3{new B{}};
  REQUIRE(ptr3);

  ptr3 = std::move(ptr2);

  // cppcheck-suppress accessMoved only in test to make sure the pointer is nullptr
  REQUIRE_FALSE(ptr2);
  REQUIRE(ptr3.get() == raw_d);
  REQUIRE(ptr3);
}