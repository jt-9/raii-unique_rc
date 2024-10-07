#ifndef WIN_GDI_HANDLE_DELETER_HPP
#define WIN_GDI_HANDLE_DELETER_HPP

#include "defs.hpp"

#include <concepts>
#include <cstddef>
#include <windows.h>


RAII_NS_BEGIN

// Deallocates GDI objects such as pen, brush, bitmaps, etc.
// nullptr indicates invalid object
template<typename Handle>
  requires std::convertible_to<Handle, HGDIOBJ>
struct gdi_delete_object_nullptr
{
  constexpr gdi_delete_object_nullptr() noexcept = default;

  template<typename U>
  raii_inline constexpr gdi_delete_object_nullptr(const gdi_delete_object_nullptr<U> &) noexcept
    requires std::is_convertible_v<U, Handle>
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(Handle h) const noexcept { DeleteObject(h); }
};

// Deallocates device context (DC) acquired via CreateDC, CreateCompatibleDC, etc.
// nullptr indicates invalid dc
struct gdi_delete_dc_nullptr
{
  constexpr gdi_delete_dc_nullptr() noexcept = default;

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(HDC h) const noexcept { DeleteDC(h); }
};

// Deallocates device context (DC) acquired via GetDC, GetWindowDC, etc.
// nullptr indicates invalid dc
struct gdi_release_wnd_dc_nullptr
{
  raii_inline constexpr gdi_release_wnd_dc_nullptr(HWND w) noexcept : wnd_{ w } {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(HDC h) const noexcept { ReleaseDC(wnd_, h); }

private:
  HWND wnd_;
};


RAII_NS_END

#endif// WIN_GDI_HANDLE_DELETER_HPP
