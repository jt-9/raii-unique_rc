#include <catch2/catch_test_macros.hpp>

#include "urc/unique_ptr.hpp"


TEST_CASE("Indirection operator* to get/set value unique_ptr<float>", "[unique_ptr][get][operator *]")
{
  constexpr auto test_number = 28.0F;
  raii::unique_ptr<float> ptr1{ new float{ test_number } };

  CHECK(ptr1.get() != nullptr);

  SECTION("Read stored value")
  {
    CHECK(*ptr1 == test_number);
    CHECK(*ptr1 == *ptr1.get());
  }

  SECTION("Write new value")
  {
    constexpr auto new_number = 8128.0F;
    REQUIRE_NOTHROW(*ptr1 = new_number);
    CHECK(*ptr1 == new_number);
    CHECK(*ptr1 == *ptr1.get());
  }
}