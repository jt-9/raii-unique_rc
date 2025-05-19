#include <catch2/catch_test_macros.hpp>


#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

#include <utility>// std::move

namespace {
struct Base
{
  Base() = default;
  Base(const Base &) = default;
  Base(Base &&) = default;

  Base &operator=(const Base &) = default;
  Base &operator=(Base &&) = default;

  virtual ~Base() = default;
};

struct Derived : public Base
{
};
}// namespace


TEST_CASE("Move assign array objects unique_ptr to default initialised unique_ptr", "[unique_ptr][operator=]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  auto *const raw_d = new Derived[3];

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  raii::unique_ptr<Derived[]> ptr1(raw_d);
  CHECK(ptr1.get() == raw_d);
  CHECK(ptr1);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  raii::unique_ptr<Derived[]> ptr2;
  CHECK(ptr2.get() == nullptr);
  REQUIRE_FALSE(ptr2);

  ptr2 = std::move(ptr1);

  // cppcheck-suppress accessMoved only in test to make sure the pointer is nullptr
  REQUIRE_FALSE(ptr1);
  CHECK(ptr2.get() == raw_d);
  CHECK(ptr2);
}