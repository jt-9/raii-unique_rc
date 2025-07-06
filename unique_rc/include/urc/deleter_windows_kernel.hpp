#ifndef RAII_DELETER_WIN_KERNEL_HPP
#define RAII_DELETER_WIN_KERNEL_HPP

#pragma once

#include "raii_defs.hpp"

#include <Windows.h>

#include <concepts>// std::convertible_to
#include <cstddef>// std::nullptr_t


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
    constexpr ~close_handle() noexcept = default;

#ifdef __cpp_static_call_operator
    raii_inline static void operator()(HANDLE h) noexcept
#else
    raii_inline void operator()(HANDLE h) const noexcept
#endif
    {
      CloseHandle(h);
    }
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

    [[nodiscard]] raii_inline static constexpr bool is_owned(Handle h) noexcept { return h != invalid(); }

    /// @brief Disabled because policy provides only typedefs and static methods
    constexpr invalid_handle_value_policy() = delete;
    constexpr ~invalid_handle_value_policy() = delete;
  };

}// namespace windows
}// namespace deleter
RAII_NS_END

#endif// RAII_DELETER_WIN_KERNEL_HPP