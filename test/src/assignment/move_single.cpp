#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

#include <utility>

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


TEST_CASE("Move assign single object unique_ptr to other unique_ptr", "[unique_ptr][operator=]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  auto *const raw_d = new Derived{};

  raii::unique_ptr<Derived> ptr1(raw_d);
  CHECK(ptr1.get() == raw_d);
  CHECK(ptr1);

  raii::unique_ptr<Derived> ptr2{ new Derived{} };
  CHECK(ptr2);

  ptr2 = std::move(ptr1);

  // cppcheck-suppress accessMoved only in test to make sure the pointer is nullptr
  CHECK_FALSE(ptr1);
  CHECK(ptr2.get() == raw_d);
  CHECK(ptr2);

  raii::unique_ptr<Base> ptr3{ new Base{} };
  CHECK(ptr3);

  ptr3 = std::move(ptr2);

  // cppcheck-suppress accessMoved only in test to make sure the pointer is nullptr
  CHECK_FALSE(ptr2);
  CHECK(ptr3.get() == raw_d);
  CHECK(ptr3);
}