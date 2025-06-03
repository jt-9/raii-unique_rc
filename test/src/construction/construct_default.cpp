#include <catch2/catch_test_macros.hpp>

#include "urc/deleter/memory_delete.hpp"
#include "urc/unique_ptr.hpp"

#include <memory>
#include <type_traits>


namespace {
template<typename T, typename D, typename... Args> constexpr bool unique_ptr_is_constructible() noexcept
{
  return std::is_constructible_v<raii::unique_ptr<T, D>, Args...>;
}

template<typename T, typename D> constexpr bool unique_ptr_is_default_constructible() noexcept
{
  return std::is_default_constructible_v<raii::unique_ptr<T, D>>;
}
}// namespace

TEST_CASE(
  "unique_ptr is not constructible with deleter reference, or array of objects is not constructible from pointer",
  "[unique_ptr][unique_ptr::unique_ptr][std::is_constructible_v]")
{
  STATIC_CHECK_FALSE(
    unique_ptr_is_default_constructible<int, raii::deleter_class_wrapper<std::default_delete<int>> &>());
  STATIC_CHECK_FALSE(unique_ptr_is_default_constructible<int, raii::default_delete<int> &>());

  // static_assert(!std::is_default_constructible<std::unique_ptr<int,
  //         void(*)(int*)>>::value, "");

  STATIC_CHECK_FALSE(
    unique_ptr_is_constructible<int, raii::deleter_class_wrapper<std::default_delete<int>> &, int *>());
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int, raii::default_delete<int> &, int *>());

  // static_assert(!std::is_constructible<std::unique_ptr<int,
  //         void(*)(int*)>, int*>::value, "");

  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  STATIC_CHECK_FALSE(
    unique_ptr_is_default_constructible<int[], raii::deleter_class_wrapper<std::default_delete<int[]>> &>());
  STATIC_CHECK_FALSE(unique_ptr_is_default_constructible<int[], raii::default_delete<int[]> &>());

  // static_assert(!std::is_default_constructible<std::unique_ptr<int[],
  //         void(*)(int*)>>::value, "");
  STATIC_CHECK_FALSE(
    unique_ptr_is_constructible<int[], raii::deleter_class_wrapper<std::default_delete<int[]>> &, int *>());
  STATIC_CHECK_FALSE(unique_ptr_is_constructible<int[], raii::default_delete<int[]> &, int *>());
  // static_assert(!std::is_constructible<std::unique_ptr<int[],
  //         void(*)(int*)>, int*>::value, "");
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
}