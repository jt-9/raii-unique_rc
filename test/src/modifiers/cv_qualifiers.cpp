#include <catch2/catch_test_macros.hpp>


#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

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


// Construction from objects with different cv-qualification

TEST_CASE("unique_ptr base class with const, volatile qualifiers, single object initialised",
  "[unique_ptr][reset][const][volatile]")
{
  constexpr A_Like_Ptr like_ptr;

  {
    raii::unique_ptr<const A> c_ptr_a;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    c_ptr_a.reset(new A);
    REQUIRE(c_ptr_a);

    // Allow conversions from user-defined pointer-like types for the single-object version
    c_ptr_a.reset(like_ptr);
    REQUIRE_FALSE(c_ptr_a);
  }

  {
    raii::unique_ptr<volatile A> v_ptr_a;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    v_ptr_a.reset(new A);
    REQUIRE(v_ptr_a);

    // Allow conversions from user-defined pointer-like types for the single-object version
    v_ptr_a.reset(like_ptr);
    REQUIRE_FALSE(v_ptr_a);
  }

  {
    raii::unique_ptr<const volatile A> cv_ptr_a;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    cv_ptr_a.reset(new A);
    REQUIRE(cv_ptr_a);

    // Allow conversions from user-defined pointer-like types for the single-object version
    cv_ptr_a.reset(like_ptr);
    REQUIRE_FALSE(cv_ptr_a);
  }
}

TEST_CASE("unique_ptr derived class with const, volatile qualifiers, single object initialised",
  "[unique_ptr][reset][const][volatile]")
{
  {
    raii::unique_ptr<const A> c_ptr_b;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    c_ptr_b.reset(new B);
    REQUIRE(c_ptr_b);
  }

  {
    raii::unique_ptr<volatile A> v_ptr_b;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    v_ptr_b.reset(new B);
    REQUIRE(v_ptr_b);
  }

  {
    raii::unique_ptr<const volatile A> cv_ptr_b;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    cv_ptr_b.reset(new B);
    REQUIRE(cv_ptr_b);
  }
}

TEST_CASE("unique_ptr base class with const, volatile qualifiers, array objects initialised",
  "[unique_ptr][reset][const][volatile]")
{
  constexpr A_Like_Ptr like_ptr;

  {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    raii::unique_ptr<const A[]> c_ptr_a;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    c_ptr_a.reset(new A[1]);
    REQUIRE(c_ptr_a);

    // Allow conversions from user-defined pointer-like types for the array version when the type is converted
    // explicitly
    c_ptr_a.reset(static_cast<A*>(like_ptr));
    REQUIRE_FALSE(c_ptr_a);
  }

  {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    raii::unique_ptr<volatile A[]> v_ptr_a;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    v_ptr_a.reset(new A[1]);
    REQUIRE(v_ptr_a);

    // Allow conversions from user-defined pointer-like types for the array version when the type is converted
    // explicitly
    v_ptr_a.reset(static_cast<A*>(like_ptr));
    REQUIRE_FALSE(v_ptr_a);
  }

  {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    raii::unique_ptr<const volatile A[]> cv_ptr_a;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    cv_ptr_a.reset(new A[1]);
    REQUIRE(cv_ptr_a);

    // Allow conversions from user-defined pointer-like types for the array version when the type is converted
    // explicitly
    cv_ptr_a.reset(static_cast<A*>(like_ptr));
    REQUIRE_FALSE(cv_ptr_a);
  }
}