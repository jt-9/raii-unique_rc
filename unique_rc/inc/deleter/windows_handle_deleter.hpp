// SmartHandle.h : Defines template class for handles
//

#ifndef WIN_HANDLE_DELETER_HPP
#define WIN_HANDLE_DELETER_HPP

#include "defs.hpp"

#include <concepts>
#include <windows.h>

RAII_NS_BEGIN

// Majority windows api's which use nullptr to mark an invalid handle
// and CloseHandle to release it, e.g. CreateThread
// read Raymond Chen - Why are HANDLE return values so inconsistent?
// https://devblogs.microsoft.com/oldnewthing/20040302-00/?p=40443
template<typename Handle>
  requires std::convertible_to<Handle, HANDLE>
struct close_handle_nullptr
{
  using handle = Handle;

  constexpr close_handle_nullptr() noexcept = default;

  template<typename U>
  raii_inline constexpr close_handle_nullptr(const close_handle_nullptr<U> &) noexcept
    requires std::is_convertible_v<U, handle>
  {}

  [[nodiscard]] raii_inline static constexpr handle invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(handle h) const noexcept { CloseHandle(h); }
};

// Use with CreateFile, CreateFileEx
template<typename Handle>
  requires std::convertible_to<Handle, HANDLE>
struct close_handle_invalid_handle_value
{
  using handle = Handle;

  constexpr close_handle_invalid_handle_value() noexcept = default;

  template<typename U>
  raii_inline constexpr close_handle_invalid_handle_value(const close_handle_invalid_handle_value<U> &) noexcept
    requires std::is_convertible_v<U, handle>
  {}

  [[nodiscard]] raii_inline static constexpr handle invalid() noexcept { return INVALID_HANDLE_VALUE; }

  raii_inline constexpr void operator()(handle h) const noexcept { CloseHandle(h); }
};

RAII_NS_END

#endif// WIN_HANDLE_DELETER_HPP
