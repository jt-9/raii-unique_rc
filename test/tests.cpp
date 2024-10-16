#include <catch2/catch_test_macros.hpp>


#include "memory_delete.hpp"
#include "mock_pointer_no_op.hpp"
#include "unique_rc.hpp"

#include <cstdint>
#include <utility>


TEST_CASE("Default initialised unique_rc<char*, memory_delete<char*>>", "[unique_rc]")
{
  const raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{};

  REQUIRE(char_rc.get() == nullptr);
  REQUIRE_FALSE(char_rc);
  // REQUIRE(factorial(10) == 3628800);
}

TEST_CASE("Move constructor from initialised unique_rc<char*, memory_delete<char*>>", "[unique_rc]")
{
  constexpr auto kChar = 'C';
  raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{ new char{ kChar } };

  CHECK(char_rc.get() != nullptr);
  CHECK(char_rc);

  decltype(char_rc) char_rc2{ std::move(char_rc) };

  REQUIRE(char_rc2);
  REQUIRE(char_rc2.get() != nullptr);
  REQUIRE(*char_rc2.get() == kChar);
}

TEST_CASE("Converting constructor of unique_rc<std::int8_t*, memory_delete<std::int8_t*>>", "[unique_rc]")
{
  constexpr auto kChar = 'A';
  raii::unique_rc<signed char *, raii::memory_delete<signed char *>> char_rc{ new signed char{ kChar } };

  REQUIRE(char_rc);

  raii::unique_rc<std::int8_t *, raii::memory_delete<std::int8_t *>> int8_rc{ std::move(char_rc) };

  REQUIRE(int8_rc);
  REQUIRE(*int8_rc.get() == kChar);
}

TEST_CASE("Access handle to get/set value unique_rc<float*, memory_delete<float*>>", "[unique_rc]")
{
  constexpr auto test_number = 28.0F;
  const raii::unique_rc<float *, raii::memory_delete<float *>> rc1{ new float{ test_number } };

  REQUIRE(rc1.get() != nullptr);

  SECTION("Read stored value") { REQUIRE(*rc1.get() == test_number); }

  SECTION("Write new value")
  {
    constexpr auto new_number = 8128.0F;
    REQUIRE_NOTHROW(*rc1.get() = new_number);
    REQUIRE(*rc1.get() == new_number);
  }
}

TEST_CASE("Release empty initialised unique_rc<float*, memory_delete<float*>>", "[unique_rc]")
{
  raii::unique_rc<float *, raii::memory_delete<float *>> default_init_rc{};

  CHECK_FALSE(default_init_rc);
  CHECK(default_init_rc.get() == nullptr);

  REQUIRE(default_init_rc.release() == nullptr);
  REQUIRE_FALSE(default_init_rc);
}

TEST_CASE("Release initialised unique_rc<float*, memory_delete<float*>>", "[unique_rc]")
{
  constexpr auto test_number = 496.0F;
  raii::unique_rc<float *, raii::memory_delete<float *>> rc1{ new float{ test_number } };

  CHECK(rc1);
  CHECK(rc1.get() != nullptr);

  decltype(rc1) init_from_release{ rc1.release() };
  REQUIRE(init_from_release);

  REQUIRE(rc1.get() == nullptr);
  REQUIRE_FALSE(rc1);
}

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

TEST_CASE("Equality of value initialised unique_rc<double*, memory_delete<double*>>", "[unique_rc]")
{
  const auto test_number = 13.11;
  const raii::unique_rc<double *, raii::memory_delete<double *>> rc1{ new double{ test_number } };

  CHECK(rc1);

  SECTION("unique_rc::operator == to other value constructed double*")
  {
    const raii::unique_rc<double *, raii::memory_delete<double *>> rc2{ new double{ 1 - test_number } };
    CHECK(rc2);

    REQUIRE_FALSE(rc1 == rc2);
    REQUIRE(rc1 != rc2);
  }

  SECTION("unique_rc::operator == to same, but non-owning unique_rc")
  {
    const raii::unique_rc<double *, mock_raii::mock_pointer_no_op<double *>> noop_rc{ rc1.get() };

    REQUIRE(rc1 == noop_rc);
    REQUIRE_FALSE(rc1 != noop_rc);
  }

  SECTION("unique_rc::operator == to default constructed unique_rc")
  {
    constexpr raii::unique_rc<double *, raii::memory_delete<double *>> default_init_rc{};
    CHECK_FALSE(default_init_rc);

    REQUIRE(rc1 != default_init_rc);
    REQUIRE(rc1 != nullptr);

    REQUIRE(default_init_rc != rc1);
    REQUIRE(nullptr != rc1);
  }
}

TEST_CASE("Three-way value initialised unique_rc<int*, memory_delete<int*>>", "[unique_rc]")
{
  const raii::unique_rc<int *, raii::memory_delete<int *>> rc1{ new int{ 73 } };

  CHECK(rc1);

  SECTION("unique_rc::operator <=> with other value constructed unique_rc")
  {
    const raii::unique_rc<int *, raii::memory_delete<int *>> rc2{ new int{ 37 } };
    CHECK(rc2);

    REQUIRE((rc1 <=> rc2) != std::strong_ordering::equal);
  }

  SECTION("unique_rc::operator <=> with same, but non-owning unique_rc")
  {
    const raii::unique_rc<int *, mock_raii::mock_pointer_no_op<int *>> noop_rc{ rc1.get() };

    REQUIRE((rc1 <=> noop_rc) == std::strong_ordering::equal);

    const auto equal_to_zero = (rc1 <=> noop_rc) == 0;
    REQUIRE(equal_to_zero);
  }

  SECTION("unique_rc::operator <=> to default constructed unique_rc")
  {
    constexpr raii::unique_rc<int *, raii::memory_delete<int *>> default_init_rc{};
    CHECK_FALSE(default_init_rc);

    REQUIRE((rc1 <=> default_init_rc) == std::strong_ordering::greater);
    REQUIRE((rc1 <=> nullptr) == std::strong_ordering::greater);

    REQUIRE((default_init_rc <=> rc1) == std::strong_ordering::less);
    REQUIRE((nullptr <=> rc1) == std::strong_ordering::less);
  }
}

TEST_CASE("Swap value initialised unique_rc<int*, memory_delete<int*>>", "[unique_rc]")
{
  const auto rc1_init_number = 24;
  raii::unique_rc<int *, raii::memory_delete<int *>> rc1{ new int{ rc1_init_number } };

  CHECK(rc1);

  SECTION("unique_rc::swap with other value constructed unique_rc")
  {
    const auto rc2_init_number = -36;
    using handle = decltype(rc1)::handle;
    raii::unique_rc<handle, raii::memory_delete<handle>> rc2{ new int{ rc2_init_number } };
    CHECK(rc2);

    auto *const ptr1 = rc1.get();
    auto *const ptr2 = rc2.get();

    rc1.swap(rc2);

    CHECK(rc1);
    CHECK(rc2);

    REQUIRE(ptr2 == rc1.get());
    REQUIRE(ptr1 == rc2.get());
  }

  SECTION("unique_rc::swap with default constructed unique_rc")
  {
    using handle = decltype(rc1)::handle;
    raii::unique_rc<handle, raii::memory_delete<handle>> default_init_rc{};
    CHECK_FALSE(default_init_rc);

    auto *const ptr1 = rc1.get();
    auto *const ptr2 = default_init_rc.get();

    rc1.swap(default_init_rc);

    REQUIRE(ptr2 == rc1.get());
    REQUIRE(ptr1 == default_init_rc.get());
  }
}