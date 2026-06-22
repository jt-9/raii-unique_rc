#include <catch2/catch_test_macros.hpp>

#include "urc/coroutine_destroy.hpp"
#include "urc/unique_coroutine_handle.hpp"

#include <coroutine>// coroutine_handle
#include <cstddef>// std::nullptr_t
#include <type_traits>


namespace {
template<typename T, typename D, typename... Args> constexpr bool unique_coroutine_is_constructible() noexcept
{ return std::is_constructible_v<raii::unique_coroutine_handle<T, D>, Args...>; }

template<typename T, typename D> constexpr bool unique_coroutine_is_default_constructible() noexcept
{ return std::is_default_constructible_v<raii::unique_coroutine_handle<T, D>>; }

struct dummy_promise_type
{
  //  constexpr void get_return_object() {}
  //  std::suspend_never initial_suspend() { return {}; }
  //  std::suspend_always final_suspend() noexcept { return {}; }
  //  void unhandled_exception() {}
  //  void return_void() {}
};
using dummy_promise_deleter_lambda_type = decltype([](std::coroutine_handle<dummy_promise_type>) {});

}// namespace

TEST_CASE("unique_coroutine_handle is constructible with deleter non-reference type",
  "[unique_coroutine_handle][unique_coroutine_handle::unique_coroutine_handle][std::is_constructible_v]")
{
  STATIC_CHECK(unique_coroutine_is_default_constructible<dummy_promise_type, raii::coroutine_destroy>());
  STATIC_CHECK(unique_coroutine_is_default_constructible<dummy_promise_type, dummy_promise_deleter_lambda_type>());

  STATIC_CHECK(unique_coroutine_is_constructible<dummy_promise_type,
    raii::coroutine_destroy,
    std::coroutine_handle<dummy_promise_type>>());

  STATIC_CHECK(unique_coroutine_is_constructible<dummy_promise_type,
    dummy_promise_deleter_lambda_type,
    std::coroutine_handle<dummy_promise_type>>());

  STATIC_CHECK(unique_coroutine_is_constructible<dummy_promise_type,
    dummy_promise_deleter_lambda_type,
    std::nullptr_t>());
}

TEST_CASE("unique_coroutine_handle is not constructible with deleter l-value reference",
  "[unique_coroutine_handle][unique_coroutine_handle::unique_coroutine_handle][std::is_constructible_v]")
{
  STATIC_CHECK_FALSE(unique_coroutine_is_default_constructible<dummy_promise_type, raii::coroutine_destroy &>());
  STATIC_CHECK_FALSE(unique_coroutine_is_default_constructible<dummy_promise_type,
    void (*)(std::coroutine_handle<dummy_promise_type>)>());

  STATIC_CHECK_FALSE(unique_coroutine_is_constructible<dummy_promise_type,
    raii::coroutine_destroy &,
    std::coroutine_handle<dummy_promise_type>>());

  STATIC_CHECK_FALSE(unique_coroutine_is_constructible<dummy_promise_type,
    void (*)(std::coroutine_handle<dummy_promise_type>),
    std::coroutine_handle<dummy_promise_type>>());

  STATIC_CHECK_FALSE(unique_coroutine_is_constructible<dummy_promise_type,
    void (*)(std::coroutine_handle<dummy_promise_type>),
    std::nullptr_t>());
}