#include <catch2/catch_test_macros.hpp>


#include "memory_delete.hpp"
#include "unique_ptr.hpp"
#include "unique_rc.hpp"


TEST_CASE("Reset initialised unique_rc<float*, memory_delete<float*>>", "[unique_rc]")
{
  constexpr auto test_number = 496.0F;
  raii::unique_rc<float *, raii::memory_delete<float *>> rc1{ new float{ test_number } };

  REQUIRE(rc1);
  REQUIRE(rc1.get() != nullptr);

  SECTION("Reset with invalid value (nullptr)")
  {
    CHECK(*rc1.get() == test_number);
    REQUIRE_NOTHROW(rc1.reset());

    REQUIRE(rc1.get() == nullptr);
    REQUIRE_FALSE(rc1);
  }

  SECTION("Reset with other constructed float")
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto *const non_owner_ptr = new float{ test_number };
    rc1.reset(non_owner_ptr);

    REQUIRE(rc1.get() == non_owner_ptr);
    REQUIRE(rc1);
  }
}

TEST_CASE("Reset empty unique_rc<float*, memory_delete<float*>> default", "[unique_rc]")
{
  raii::unique_rc<float *, raii::memory_delete<float *>> float_rc{};

  REQUIRE_FALSE(float_rc);
  REQUIRE(float_rc.get() == nullptr);

  REQUIRE_NOTHROW(float_rc.reset());

  REQUIRE(float_rc.get() == nullptr);
  REQUIRE_FALSE(float_rc);
}


struct A;

struct B
{
  raii::unique_ptr<A> a;
};

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct A
{
  B *b = nullptr;

  ~A() { REQUIRE(b->a != nullptr); }
};

TEST_CASE("Reset unique_ptr<A> to new A", "[unique_ptr::reset]")
{
  // NOLINTNEXTLINE(readability-identifier-length)
  B b;

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  b.a.reset(new A);
  b.a->b = &b;
}

struct C;

struct D
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  raii::unique_ptr<C[]> c;
};

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct C
{
  D *d = nullptr;

  ~C() { REQUIRE(d->c != nullptr); }
};

TEST_CASE("Reset unique_ptr<C[]> to new C[1]", "[unique_ptr::reset]")
{
  // NOLINTNEXTLINE(readability-identifier-length)
  D d;

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  d.c.reset(new C[1]);
  d.c[0].d = &d;
}