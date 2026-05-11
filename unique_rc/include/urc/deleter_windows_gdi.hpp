#ifndef RAII_DELETER_WIN_GDI_HPP
#define RAII_DELETER_WIN_GDI_HPP

#include "raii_defs.hpp"

#include <Windows.h>

#include <concepts>// std::convertible_to


RAII_NS_BEGIN

namespace deleter {
namespace windows {

  /**
   * @brief Deallocates device context (DC) acquired via CreateDC, CreateCompatibleDC, etc.
   * @note example: `unique_rc<HDC, gdi_delete_dc, raii::resolve_handle_type, std::nullptr_t>`
   * or short form `unique_rc<HDC, gdi_delete_dc>`
   **/
  struct gdi_delete_dc
  {
    constexpr gdi_delete_dc() noexcept = default;

// constexpr generates error - constexpr function doesn't evaluate at compile time
#ifdef __cpp_static_call_operator
    raii_inline static void operator()(HDC hdc) noexcept
#else
    raii_inline void operator()(HDC hdc) const noexcept
#endif
    { DeleteDC(hdc); }
  };// gdi_delete_dc

  // Deallocates GDI objects such as pen, brush, bitmaps, etc.
  // nullptr indicates invalid object
  struct gdi_delete_object
  {
    constexpr gdi_delete_object() noexcept = default;

#ifdef __cpp_static_call_operator
    raii_inline static void operator()(HGDIOBJ hgdi) noexcept
#else
    raii_inline void operator()(HGDIOBJ hgdi) const noexcept
#endif
    { DeleteObject(hgdi); }
  };// gdi_delete_object


  struct gdi_release_dc
  {
    struct handle
    {
      HDC hdc;
      HWND hwnd;

      // NOLINTNEXTLINE(readability-identifier-length, bugprone-easily-swappable-parameters, cppcoreguidelines-pro-type-member-init, hicpp-member-init)
      raii_inline constexpr handle(HDC dc, HWND wnd) noexcept : hdc{ dc }, hwnd{ wnd } {}

      raii_inline constexpr handle() noexcept : handle(nullptr, nullptr) {}

      constexpr handle(const handle &) noexcept = default;
      constexpr handle(handle &&) noexcept = default;

      constexpr handle &operator=(const handle &) noexcept = default;
      constexpr handle &operator=(handle &&) noexcept = default;

      constexpr ~handle() noexcept = default;

      [[nodiscard]] raii_inline constexpr HDC operator*() const noexcept { return hdc; }

      [[nodiscard]] friend raii_inline constexpr bool operator==(const handle &lhs, const handle &rhs) noexcept
      { return (lhs.hdc == rhs.hdc) && (lhs.hwnd == rhs.hwnd); }

      friend raii_inline constexpr void swap(handle &lhs, handle &rhs) noexcept
      {
        std::ranges::swap(lhs.hdc, rhs.hdc);
        std::ranges::swap(lhs.hwnd, rhs.hwnd);
      }
    };// handle


// constexpr generates error - constexpr function doesn't evaluate at compile time
#ifdef __cpp_static_call_operator
    raii_inline static void operator()(handle hnd) noexcept
#else
    raii_inline void operator()(handle hnd) const noexcept
#endif
    { ReleaseDC(hnd.hwnd, hnd.hdc); }
  };// gdi_release_dc

  template<typename Handle, typename Invalid = Handle> struct dc_wnd_invalid_handle_policy
  {
    using invalid_type = Invalid;

    [[nodiscard]] raii_inline static constexpr invalid_type invalid() noexcept { return {}; }

    [[nodiscard]] raii_inline static constexpr bool is_owned(Handle hnd) noexcept { return static_cast<bool>(hnd.hdc); }

    /// @brief Disabled because policy provides only typedefs and static methods
    constexpr dc_wnd_invalid_handle_policy() = delete;
    constexpr ~dc_wnd_invalid_handle_policy() = delete;

    constexpr dc_wnd_invalid_handle_policy(const dc_wnd_invalid_handle_policy &) = delete;
    constexpr dc_wnd_invalid_handle_policy &operator=(const dc_wnd_invalid_handle_policy &) = delete;

    constexpr dc_wnd_invalid_handle_policy(dc_wnd_invalid_handle_policy &&) = delete;
    constexpr dc_wnd_invalid_handle_policy &operator=(dc_wnd_invalid_handle_policy &&) = delete;
  };// dc_wnd_invalid_handle_policy


  struct gdi_restore_dc
  {
    struct handle
    {
      HDC hdc;
      int state;

      // NOLINTNEXTLINE(readability-identifier-length, bugprone-easily-swappable-parameters, cppcoreguidelines-pro-type-member-init, hicpp-member-init)
      raii_inline constexpr handle(HDC dc, int nstate) noexcept : hdc{ dc }, state{ nstate } {}

      raii_inline constexpr handle() noexcept : handle(nullptr, 0) {}

      constexpr handle(const handle &) noexcept = default;
      constexpr handle &operator=(const handle &) noexcept = default;

      constexpr handle(handle &&) noexcept = default;
      constexpr handle &operator=(handle &&) noexcept = default;

      constexpr ~handle() noexcept = default;

      // [[nodiscard]] raii_inline constexpr HDC operator*() const noexcept { return hdc; }

      [[nodiscard]] friend raii_inline constexpr bool operator==(const handle &lhs, const handle &rhs) noexcept
      { return (lhs.state == rhs.state) && (lhs.hdc == rhs.hdc); }

      friend raii_inline constexpr void swap(handle &lhs, handle &rhs) noexcept
      {
        std::ranges::swap(lhs.hdc, rhs.hdc);
        std::ranges::swap(lhs.state, rhs.state);
      }
    };


// constexpr generates error - constexpr function doesn't evaluate at compile time
#ifdef __cpp_static_call_operator
    raii_inline static void operator()(handle hnd) noexcept
#else
    raii_inline void operator()(handle hnd) const noexcept
#endif
    { RestoreDC(hnd.hdc, hnd.state); }
  };// gdi_restore_dc

  template<typename Handle, typename Invalid = Handle> struct dc_state_invalid_handle_policy
  {
    using invalid_type = Invalid;

    [[nodiscard]] raii_inline static constexpr invalid_type invalid() noexcept { return {}; }

    [[nodiscard]] raii_inline static constexpr bool is_owned(Handle hnd) noexcept
    { return static_cast<bool>(hnd.hdc) && (hnd.state != 0); }

    /// @brief Disabled because policy provides only typedefs and static methods
    constexpr dc_state_invalid_handle_policy() = delete;
    constexpr ~dc_state_invalid_handle_policy() = delete;

    constexpr dc_state_invalid_handle_policy(const dc_state_invalid_handle_policy &) = delete;
    constexpr dc_state_invalid_handle_policy &operator=(const dc_state_invalid_handle_policy &) = delete;

    constexpr dc_state_invalid_handle_policy(dc_state_invalid_handle_policy &&) = delete;
    constexpr dc_state_invalid_handle_policy &operator=(dc_state_invalid_handle_policy &&) = delete;
  };// dc_state_invalid_handle_policy


  // Selects GDI objects such as pen, brush, bitmap, etc.
  // nullptr indicates invalid object
  template<typename GdiObjHandle = HGDIOBJ>
    requires std::convertible_to<GdiObjHandle, HGDIOBJ>
  struct gdi_select_object
  {
    struct handle
    {
      HDC hdc;
      GdiObjHandle hobj;

      raii_inline constexpr handle(HDC dc, GdiObjHandle obj) noexcept : hdc{ dc }, hobj{ obj } {}

      raii_inline constexpr handle() noexcept : handle(nullptr, nullptr) {}

      constexpr handle(const handle &) noexcept = default;
      constexpr handle(handle &&) noexcept = default;

      constexpr handle &operator=(const handle &) noexcept = default;
      constexpr handle &operator=(handle &&) noexcept = default;

      constexpr ~handle() noexcept = default;

      // [[nodiscard]] raii_inline constexpr HDC operator*() const noexcept { return hdc; }

      [[nodiscard]] friend raii_inline constexpr bool operator==(const handle &lhs, const handle &rhs) noexcept
      { return (lhs.hobj == rhs.hobj) && (lhs.hdc == rhs.hdc); }

      friend raii_inline constexpr void swap(handle &lhs, handle &rhs) noexcept
      {
        std::ranges::swap(lhs.hdc, rhs.hdc);
        std::ranges::swap(lhs.hobj, rhs.hobj);
      }
    };// handle

    constexpr gdi_select_object() noexcept = default;
    constexpr ~gdi_select_object() noexcept = default;

#ifdef __cpp_static_call_operator
    raii_inline static void operator()(handle hnd) noexcept
#else
    raii_inline void operator()(handle hnd) const noexcept
#endif
    { SelectObject(hnd.hdc, hnd.hobj); }
  };// gdi_select_object

  template<typename Handle, typename Invalid = Handle> struct dc_gdiobj_invalid_handle_policy
  {
    using invalid_type = Invalid;

    [[nodiscard]] raii_inline static constexpr invalid_type invalid() noexcept { return {}; }

    [[nodiscard]] raii_inline static constexpr bool is_owned(Handle hnd) noexcept
    { return static_cast<bool>(hnd.hobj); }

    /// @brief Disabled because policy provides only typedefs and static methods
    constexpr dc_gdiobj_invalid_handle_policy() = delete;
    constexpr ~dc_gdiobj_invalid_handle_policy() = delete;

    constexpr dc_gdiobj_invalid_handle_policy(const dc_gdiobj_invalid_handle_policy &) = delete;
    constexpr dc_gdiobj_invalid_handle_policy &operator=(const dc_gdiobj_invalid_handle_policy &) = delete;

    constexpr dc_gdiobj_invalid_handle_policy(dc_gdiobj_invalid_handle_policy &&) = delete;
    constexpr dc_gdiobj_invalid_handle_policy &operator=(dc_gdiobj_invalid_handle_policy &&) = delete;
  };// dc_gdiobj_invalid_handle_policy

}// namespace windows
}// namespace deleter

RAII_NS_END

#endif// RAII_DELETER_WIN_GDI_HPP