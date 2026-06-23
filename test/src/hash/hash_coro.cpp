#include <catch2/catch_test_macros.hpp>

#include "urc/unique_coroutine_handle.hpp"

#include <coroutine>
#include <functional>// std::hash


TEST_CASE("std::hash for unique_coroutine_handle of type Empty", "[unique_coroutine_handle][hash]")
{
  struct Empty
  {
  };

  const raii::unique_coroutine_handle<Empty> coro_handle{ std::coroutine_handle<Empty>{} };
  const std::hash<raii::unique_coroutine_handle<Empty>> hu0;
  const std::hash<raii::unique_coroutine_handle<Empty>::handle> hp0;

  CHECK(hu0(coro_handle) == hp0(coro_handle.get()));
}