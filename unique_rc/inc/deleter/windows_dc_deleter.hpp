#ifndef WIN_GDI_DC_DELETER_HPP
#define WIN_GDI_DC_DELETER_HPP

#include "raii_defs.hpp"

#include <concepts>
#include <cstddef>

#include <Windows.h>


RAII_NS_BEGIN


// Deallocates device context (DC) acquired via CreateDC, CreateCompatibleDC, etc.
// nullptr indicates invalid dc
struct gdi_delete_dc_nullptr
{
  constexpr gdi_delete_dc_nullptr() noexcept = default;

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  // constexpr generates error - constexpr function doesn't evaluate at compile time
  raii_inline void operator()(HDC h) const noexcept { DeleteDC(h); }
};

// Deallocates device context (DC) acquired via GetDC, GetWindowDC, etc.
// nullptr indicates invalid dc
struct gdi_release_wnd_dc_nullptr
{
  raii_inline explicit constexpr gdi_release_wnd_dc_nullptr(HWND w) noexcept : wnd_{ w } {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  // constexpr generates error - constexpr function doesn't evaluate at compile time
  raii_inline void operator()(HDC h) const noexcept { ReleaseDC(wnd_, h); }

private:
  HWND wnd_;
};


RAII_NS_END

#endif// WIN_GDI_DC_DELETER_HPP
