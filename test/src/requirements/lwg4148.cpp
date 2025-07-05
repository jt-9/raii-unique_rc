#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

#include <type_traits>

namespace {
struct deleter
{
  using pointer = long *;
  void operator()(pointer /*unused*/) const {}
};

using UPtr = raii::unique_ptr<const int, deleter>;
}// namespace

#ifdef __cpp_lib_reference_from_temporary
TEST_CASE("LWG 4148 unique_ptr::operator* should not allow dangling references", "[unique_ptr::operator *]")
{
  // 4148. unique_ptr::operator* should not allow dangling references
  long lvar = 0;
  // NOLINTNEXTLINE(misc-const-correctness)
  UPtr iptr{ &lvar };

  using ElemRefT = typename std::add_lvalue_reference_t<UPtr::element_type>;
  using DerefT = decltype(*iptr.get());
  STATIC_REQUIRE(std::reference_converts_from_temporary_v<ElemRefT, DerefT>);

  // NOLINTNEXTLINE(misc-const-correctness)
  //[[maybe_unused]] int ivar = *iptr;// c++23 error, static_assert fails in unique_ptr::operator *
}
#endif