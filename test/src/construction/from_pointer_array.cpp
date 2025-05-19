#include <catch2/catch_test_macros.hpp>

// #include "memory_delete.hpp"
#include "unique_ptr.hpp"

#include <type_traits>


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

constexpr auto constArraySize = 3;
}// namespace


TEST_CASE("unique_ptr of array of objects constructed via new[]", "[unique_ptr][unique_ptr::unique_ptr]")
{
  reset_counters<A>();
  reset_counters<B>();

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  const raii::unique_ptr<A[]> A_default;
  CHECK(A_default.get() == 0);
  CHECK(A::ctor_count == 0);
  CHECK(A::dtor_count == 0);
  CHECK(B::ctor_count == 0);
  CHECK(B::dtor_count == 0);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  const raii::unique_ptr<A[]> A_from_A(new A[constArraySize]);
  CHECK(A_from_A.get() != 0);
  CHECK(A::ctor_count == constArraySize);
  CHECK(A::dtor_count == 0);
  CHECK(B::ctor_count == 0);
  CHECK(B::dtor_count == 0);
}

TEST_CASE("unique_ptr of array of objects constructed from raw pointer", "[unique_ptr][unique_ptr::unique_ptr]")
{
  reset_counters<A>();
  reset_counters<B>();

  A *const A_default = nullptr;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  const raii::unique_ptr<A[]> ptr1{ A_default };
  CHECK(ptr1.get() == nullptr);
  CHECK(A::ctor_count == 0);
  CHECK(A::dtor_count == 0);
  CHECK(B::ctor_count == 0);
  CHECK(B::dtor_count == 0);

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  A *const A_from_A = new A[constArraySize];
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  const raii::unique_ptr<A[]> ptr2{ A_from_A };
  CHECK(ptr2.get() == A_from_A);
  CHECK(A::ctor_count == constArraySize);
  CHECK(A::dtor_count == 0);
  CHECK(B::ctor_count == 0);
  CHECK(B::dtor_count == 0);
}

TEST_CASE("unique_ptr of array of derived objects shall not be constructible from array of objects of base",
  "[unique_ptr][unique_ptr::unique_ptr]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(std::is_constructible_v<raii::unique_ptr<B[]>, A[constArraySize]>);
  // raii::unique_ptr<B[]> B_from_A{
  //   new A[constArraySize]
  // };//{ dg-error "candidate constructor (the implicit copy constructor) not viable: no known conversion from 'A *' to
  //   //'const unique_ptr<B[]>" }
}