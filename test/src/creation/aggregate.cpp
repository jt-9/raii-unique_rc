#include <catch2/catch_test_macros.hpp>

#include "unique_ptr.hpp"
// #include <memory>

namespace {
struct aggressive_aggregate
{
  int a;
  int b;
};
}// namespace


TEST_CASE("raii::make_unique aggregate type", "[unique_ptr][make_unique][aggregate]")
{
  const auto ag_ptr1 = raii::make_unique<aggressive_aggregate>(1, 2);
  CHECK(ag_ptr1->a == 1);
  CHECK(ag_ptr1->b == 2);

  // const auto ag_ptr2 = std::make_unique<aggressive_aggregate>(1);
  // error: missing field 'b' initializer [clang-diagnostic-missing-field-initializers,-warnings-as-errors]
  const auto ag_ptr2 = raii::make_unique<aggressive_aggregate>(1, 0);
  CHECK(ag_ptr2->a == 1);
  CHECK(ag_ptr2->b == 0);

  const auto ag_ptr3 = raii::make_unique<aggressive_aggregate>();
  CHECK(ag_ptr3->a == 0);
  CHECK(ag_ptr3->b == 0);
}