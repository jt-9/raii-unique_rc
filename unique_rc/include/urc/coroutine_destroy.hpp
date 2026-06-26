#ifndef COROUTINE_DESTROY_HPP
#define COROUTINE_DESTROY_HPP

#include "raii_defs.hpp"

#include <coroutine>


RAII_NS_BEGIN

template<typename CoroHandle, typename Invalid> struct coro_invalid_handle_policy
{
  using invalid_type = Invalid;

  [[nodiscard]] raii_inline static constexpr invalid_type invalid() noexcept { return {}; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(CoroHandle hnd) noexcept { return static_cast<bool>(hnd); }


  /// @brief Disabled because policy provides only typedefs and static methods
  constexpr coro_invalid_handle_policy() = delete;
  constexpr ~coro_invalid_handle_policy() = delete;

  constexpr coro_invalid_handle_policy(const coro_invalid_handle_policy &) = delete;
  constexpr coro_invalid_handle_policy& operator=(const coro_invalid_handle_policy &) = delete;

  constexpr coro_invalid_handle_policy(coro_invalid_handle_policy &&) = delete;
  constexpr coro_invalid_handle_policy& operator=(coro_invalid_handle_policy &&) = delete;
};

struct coroutine_destroy
{
  constexpr coroutine_destroy() noexcept = default;

  template<typename Promise>
#ifdef __cpp_static_call_operator
  // False poisitive, guarded by feature #ifdef __cpp_static_call_operator
  // NOLINTNEXTLINE(clang-diagnostic-c++23-extensions)
  raii_inline static void operator()(std::coroutine_handle<Promise> hnd) noexcept
#else
  raii_inline void operator()(std::coroutine_handle<Promise> hnd) const noexcept
#endif
  { hnd.destroy(); }
};

RAII_NS_END

#endif// COROUTINE_DESTROY_HPP