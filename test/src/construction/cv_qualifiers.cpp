#include <catch2/catch_test_macros.hpp>


#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

#include <utility>// std::move

namespace {
struct A
{
  A() = default;
  A(const A &) = default;
  A(A &&) = default;

  A &operator=(const A &) = default;
  A &operator=(A &&) = default;

  virtual ~A() = default;
};

struct B : A
{
};

struct A_Like_Ptr
{
  // NOLINTNEXTLINE(hicpp-explicit-conversions) // intended implicit conversion
  operator A *() const { return nullptr; }
};
}// namespace


TEST_CASE("unique_ptr base class with const, volatile, initialised single object base object",
  "[unique_ptr][unique_ptr::unique_ptr][const][volatile]")
{
  const raii::unique_ptr<const A> constA{ new A };
  CHECK(constA);
  CHECK(constA.get());

  const raii::unique_ptr<volatile A> volatA{ new A };
  CHECK(volatA);
  CHECK(volatA.get());

  const raii::unique_ptr<const volatile A> cvA{ new A };
  CHECK(cvA);
  CHECK(cvA.get());
}

TEST_CASE("unique_ptr derived class with const, volatile, initialised single object derived object",
  "[unique_ptr][unique_ptr::unique_ptr][const][volatile]")
{
  const raii::unique_ptr<const A> constB{ new B };
  CHECK(constB);
  CHECK(constB.get());

  const raii::unique_ptr<volatile A> volatB{ new B };
  CHECK(volatB);
  CHECK(volatB.get());

  const raii::unique_ptr<const volatile A> cvB{ new B };
  CHECK(cvB);
  CHECK(cvB.get());
}

TEST_CASE("unique_ptr base class with const, volatile, move from default initialised base object",
  "[unique_ptr][unique_ptr::unique_ptr][const][volatile]")
{
  // NOLINTNEXTLINE(readability-isolate-declaration)
  raii::unique_ptr<A> upA1, upA2, upA3;

  const raii::unique_ptr<const A> constA{ std::move(upA1) };
  const raii::unique_ptr<volatile A> volatA{ std::move(upA2) };
  const raii::unique_ptr<const volatile A> cvA{ std::move(upA3) };
}

TEST_CASE("unique_ptr base class with const, volatile, move from default initialised derived object",
  "[unique_ptr][unique_ptr::unique_ptr][const][volatile]")
{
  // NOLINTNEXTLINE(readability-isolate-declaration)
  raii::unique_ptr<B> upB1, upB2, upB3;

  const raii::unique_ptr<const A> constA{ std::move(upB1) };
  const raii::unique_ptr<volatile A> volatA{ std::move(upB2) };
  const raii::unique_ptr<const volatile A> cvA{ std::move(upB3) };
}

TEST_CASE("unique_ptr base class array objects with const, volatile, initialised base array objects",
  "[unique_ptr][unique_ptr::unique_ptr][const][volatile]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  const raii::unique_ptr<const A[]> constA{ new A[1] };
  CHECK(constA);
  CHECK(constA.get());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  const raii::unique_ptr<volatile A[]> volatA{ new A[1] };
  CHECK(volatA);
  CHECK(volatA.get());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  const raii::unique_ptr<const volatile A[]> cvA{ new A[1] };
  CHECK(cvA);
  CHECK(cvA.get());
}

TEST_CASE("unique_ptr base class with const, volatile, move from default initialised base array objects",
  "[unique_ptr][unique_ptr::unique_ptr][const][volatile]")
{
  // NOLINTNEXTLINE
  raii::unique_ptr<A[]> upA1, upA2, upA3;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  const raii::unique_ptr<const A[]> constA(std::move(upA1));

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  const raii::unique_ptr<volatile A[]> volatA(std::move(upA2));

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  const raii::unique_ptr<const volatile A[]> cvA(std::move(upA3));
}

TEST_CASE("unique_ptr base class with const, volatile, array objects initialised",
  "[unique_ptr][unique_ptr::unique_ptr][const][volatile]")
{
  constexpr A_Like_Ptr like_ptr;

  {
    const raii::unique_ptr<A> ptr_a{ like_ptr };
    REQUIRE_FALSE(ptr_a);

    // Allow conversions from user-defined pointer-like types for the array version when the type is converted
    // explicitly
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    const raii::unique_ptr<A[]> ptr_aa{ static_cast<A *>(like_ptr) };
    REQUIRE_FALSE(ptr_aa);
  }

  {
    const raii::unique_ptr<const A> c_ptr_a{ like_ptr };
    REQUIRE_FALSE(c_ptr_a);

    // Allow conversions from user-defined pointer-like types for the array version when the type is converted
    // explicitly
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    const raii::unique_ptr<const A[]> c_ptr_aa{ static_cast<A *>(like_ptr) };
    REQUIRE_FALSE(c_ptr_aa);
  }

  {
    const raii::unique_ptr<const A> v_ptr_a{ like_ptr };
    REQUIRE_FALSE(v_ptr_a);
    
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    const raii::unique_ptr<volatile A[]> v_ptr_aa{static_cast<A *>(like_ptr)};
    REQUIRE_FALSE(v_ptr_aa);
  }

  {
    const raii::unique_ptr<const volatile A> cv_ptr_a{ like_ptr };
    REQUIRE_FALSE(cv_ptr_a);
    
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    const raii::unique_ptr<const volatile A[]> cv_ptr_aa{static_cast<A *>(like_ptr)};
    REQUIRE_FALSE(cv_ptr_aa);
  }
}