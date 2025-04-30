#include <catch2/catch_test_macros.hpp>

// #include "memory_delete.hpp"
#include "unique_ptr.hpp"

namespace {
struct A
{
  A() { ++ctor_count; }
  virtual ~A() { ++dtor_count; }

  A(const A &) = default;
  A(A &&) = default;

  A &operator=(const A &) = default;
  A &operator=(A &&) = default;

  static long ctor_count;
  static long dtor_count;
};
long A::ctor_count = 0;
long A::dtor_count = 0;

struct B : A
{
  B() { ++ctor_count; }
  ~B() override { ++dtor_count; }

  B(const B &) = default;
  B(B &&) = default;

  B &operator=(const B &) = default;
  B &operator=(B &&) = default;

  // cppcheck-suppress duplInheritedMember it's okay in tests, but generally a bad practice
  static long ctor_count;
  // cppcheck-suppress duplInheritedMember it's okay in tests, but generally a bad practice
  static long dtor_count;
};
long B::ctor_count = 0;
long B::dtor_count = 0;


template<typename T> void reset_counters() noexcept
{
  T::ctor_count = 0;
  T::dtor_count = 0;
}
}// namespace


TEST_CASE("unique_ptr of single object constructed via new", "[unique_ptr][unique_ptr::unique_ptr]")
{
  reset_counters<A>();
  reset_counters<B>();

  constexpr raii::unique_ptr<A> A_default;
  REQUIRE(A_default.get() == 0);
  REQUIRE(A::ctor_count == 0);
  REQUIRE(A::dtor_count == 0);
  REQUIRE(B::ctor_count == 0);
  REQUIRE(B::dtor_count == 0);

  const raii::unique_ptr<A> A_from_A{ new A };
  REQUIRE(A_from_A.get() != 0);
  REQUIRE(A::ctor_count == 1);
  REQUIRE(A::dtor_count == 0);
  REQUIRE(B::ctor_count == 0);
  REQUIRE(B::dtor_count == 0);

  const raii::unique_ptr<A> A_from_B{ new B };
  REQUIRE(A_from_B.get() != 0);
  REQUIRE(A::ctor_count == 2);
  REQUIRE(A::dtor_count == 0);
  REQUIRE(B::ctor_count == 1);
  REQUIRE(B::dtor_count == 0);
}

TEST_CASE("unique_ptr of single object constructed from raw pointer", "[unique_ptr][unique_ptr::unique_ptr]")
{
  reset_counters<A>();
  reset_counters<B>();

  A *const A_default = nullptr;
  const raii::unique_ptr<A> ptr1{ A_default };
  REQUIRE(ptr1.get() == nullptr);
  REQUIRE(A::ctor_count == 0);
  REQUIRE(A::dtor_count == 0);
  REQUIRE(B::ctor_count == 0);
  REQUIRE(B::dtor_count == 0);

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  A *const A_from_A = new A;
  const raii::unique_ptr<A> ptr2{ A_from_A };
  REQUIRE(ptr2.get() == A_from_A);
  REQUIRE(A::ctor_count == 1);
  REQUIRE(A::dtor_count == 0);
  REQUIRE(B::ctor_count == 0);
  REQUIRE(B::dtor_count == 0);
}