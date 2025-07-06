#include <catch2/catch_test_macros.hpp>

#include "urc/memory_delete.hpp"
#include "urc/unique_ptr.hpp"
#include "urc/unique_rc.hpp"


namespace {

struct A;

struct B
{
  raii::unique_ptr<A> a;
};

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct A
{
  B *b = nullptr;

  ~A() { CHECK(b->a != nullptr); }
};

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

  ~C() { CHECK(d->c != nullptr); }
};

}// namespace

TEST_CASE("Reset unique_ptr<A> to new A", "[unique_ptr][reset]")
{
  // NOLINTNEXTLINE(readability-identifier-length)
  B b;

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  b.a.reset(new A);
  b.a->b = &b;
}

TEST_CASE("Reset unique_ptr<C[]> to new C[1]", "[unique_ptr][reset]")
{
  // NOLINTNEXTLINE(readability-identifier-length)
  D d;

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  d.c.reset(new C[1]);
  d.c[0].d = &d;
}

TEST_CASE("Reset initialised unique_rc<float*, memory_delete<float*>>", "[unique_rc][reset]")
{
  constexpr auto test_number = 496.0F;
  raii::unique_rc<float *, raii::memory_delete<float *>> rc1{ new float{ test_number } };

  CHECK(rc1);
  CHECK(rc1.get() != nullptr);

  SECTION("Reset with invalid value (nullptr)")
  {
    CHECK(*rc1.get() == test_number);
    REQUIRE_NOTHROW(rc1.reset());

    CHECK(rc1.get() == nullptr);
    CHECK_FALSE(rc1);
  }

  SECTION("Reset with other constructed float")
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto *const non_owner_ptr = new float{ test_number };
    rc1.reset(non_owner_ptr);

    CHECK(rc1.get() == non_owner_ptr);
    CHECK(rc1);
  }
}

TEST_CASE("Reset empty unique_rc<float*, memory_delete<float*>> default", "[unique_rc][reset]")
{
  raii::unique_rc<float *, raii::memory_delete<float *>> float_rc{};

  CHECK_FALSE(float_rc);
  CHECK(float_rc.get() == nullptr);

  REQUIRE_NOTHROW(float_rc.reset());

  CHECK(float_rc.get() == nullptr);
  CHECK_FALSE(float_rc);
}

TEST_CASE("Reset initialised unique_ptr<float>", "[unique_ptr][reset]")
{
  constexpr auto test_number = 8128.0F;
  raii::unique_ptr<float> ptr1{ new float{ test_number } };

  CHECK(ptr1);
  CHECK(ptr1.get() != nullptr);
  CHECK(*ptr1 == test_number);

  SECTION("Reset with invalid value (nullptr)")
  {
    REQUIRE_NOTHROW(ptr1.reset());

    CHECK(ptr1.get() == nullptr);
    CHECK_FALSE(ptr1);
  }

  SECTION("Reset with other constructed float")
  {
    constexpr auto test_number2 = -6.0F;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto *const non_owner_ptr = new float{ test_number2 };
    ptr1.reset(non_owner_ptr);

    CHECK(ptr1.get() == non_owner_ptr);
    CHECK(ptr1);
  }
}


namespace {
struct D2
{
  static int count;

  void operator()(int const *ptr) const
  {
    ++count;

    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete ptr;
  }
};
int D2::count = 0;
}// namespace

TEST_CASE("Test deleter operator() is called as expected", "[unique_ptr][reset]")
{
  raii::unique_ptr<int, D2> iptr;

  iptr.reset();
  CHECK(D2::count == 0);

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  iptr.reset(new int);
  CHECK(D2::count == 0);

  iptr.reset(iptr.get());
  CHECK(D2::count == 1);

  iptr.release();
  CHECK(D2::count == 1);
}