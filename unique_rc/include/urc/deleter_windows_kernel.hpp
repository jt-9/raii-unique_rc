#ifndef RAII_DELETER_WIN_KERNEL_HPP
#define RAII_DELETER_WIN_KERNEL_HPP

#include "raii_defs.hpp"

#include <Windows.h>


RAII_NS_BEGIN

namespace deleter {
namespace windows {

  // Majority windows api's which use nullptr to mark an invalid handle
  // and CloseHandle to release it, e.g. CreateThread
  // read Raymond Chen - Why are HANDLE return values so inconsistent?
  // https://devblogs.microsoft.com/oldnewthing/20040302-00/?p=40443

  struct close_handle
  {
    constexpr close_handle() noexcept = default;

#ifdef __cpp_static_call_operator
    // False poisitive, guarded by feature #ifdef __cpp_static_call_operator
    // NOLINTNEXTLINE(clang-diagnostic-c++23-extensions)
    raii_inline static void operator()(HANDLE hnd) noexcept
#else
    raii_inline void operator()(HANDLE hnd) const noexcept
#endif
    { CloseHandle(hnd); }
  };


  /**
   * @brief Use with CreateFile, CreateFileEx
   * @tparam Handle type of system resource, usually HANDLE
   * @tparam Invalid invalid handle type, same as Handle
   **/
  template<typename Handle, typename Invalid> struct invalid_handle_value_policy
  {
    using invalid_type = Invalid;

    [[nodiscard]] raii_inline static constexpr invalid_type invalid() noexcept { return INVALID_HANDLE_VALUE; }

    [[nodiscard]] raii_inline static constexpr bool is_owned(Handle hnd) noexcept { return hnd != invalid(); }

    /// @brief Disabled because policy provides only typedefs and static methods
    constexpr invalid_handle_value_policy() = delete;
    constexpr ~invalid_handle_value_policy() = delete;

    constexpr invalid_handle_value_policy(const invalid_handle_value_policy &) = delete;
    constexpr invalid_handle_value_policy &operator=(const invalid_handle_value_policy &) = delete;

    constexpr invalid_handle_value_policy(invalid_handle_value_policy &&) = delete;
    constexpr invalid_handle_value_policy &operator=(invalid_handle_value_policy &&) = delete;
  };

}// namespace windows
}// namespace deleter

RAII_NS_END

#endif// RAII_DELETER_WIN_KERNEL_HPP