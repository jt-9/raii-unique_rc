#include <catch2/catch_test_macros.hpp>

#include "urc/unique_coroutine_handle.hpp"

#include <type_traits>// std::is_nothrow_move_constructible_v


TEST_CASE("unique_coroutine_handle must be noexcept move constructible",
  "[unique_coroutine_handle][unique_coroutine_handle::unique_coroutine_handle][noexcept][constexpr]")
{
  struct Empty
  {
  };

  STATIC_CHECK(std::is_nothrow_move_constructible_v<raii::unique_coroutine_handle<Empty>>);
}