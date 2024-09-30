// SmartHandle.h : Defines template class for handles
//

#ifndef WIN_GDI_HANDLE_DELETER_HPP
#define WIN_GDI_HANDLE_DELETER_HPP

#include "defs.hpp"

#include <concepts>
#include <windows.h>
#include <cstddef>

RAII_NS_BEGIN

// Use to release GDI objects pen, brush, bitmaps, etc. nullptr indicates invalid object
template<typename Handle>
  requires std::convertible_to<Handle, HGDIOBJ>
struct gdi_delete_object_nullptr
{
  using handle = Handle;

  constexpr gdi_delete_object_nullptr() noexcept = default;

  template<typename U>
  raii_inline constexpr gdi_delete_object_nullptr(const gdi_delete_object_nullptr<U> &) noexcept
    requires std::is_convertible_v<U, handle>
  {}

  [[nodiscard]] raii_inline static constexpr nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(handle h) const noexcept { DeleteObject(h); }
};

// Use to release DC acquired via CreateDC, CreateCompatibleDC, etc. nullptr indicates invalid dc
struct gdi_delete_dc_nullptr
{
  using handle = HDC;

  constexpr gdi_delete_dc_nullptr() noexcept = default;

  [[nodiscard]] raii_inline static constexpr nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(handle h) const noexcept { DeleteDC(h); }
};

struct gdi_release_wnd_dc_nullptr
{
  using handle = HDC;

  raii_inline constexpr gdi_release_wnd_dc_nullptr(HWND w) noexcept 
  : wnd_{ w } {}

  [[nodiscard]] raii_inline static constexpr nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(handle h) const noexcept { ReleaseDC(wnd_, h); }

private:
  HWND wnd_;
};


// Can be used for bitmaps, pens, brushes, etc.
// template< typename Handle = HGDIOBJ >
// using GDIObjectHandle = UniqueHandle<HGDIOBJPolicy<Handle>>;
//
// using UniqueDCHandle = UniqueHandle< UniqueHDCPolicy >;
// using ViewDCHandle = UniqueHandle< GetHDCPolicy >;

RAII_NS_END

#endif// WIN_GDI_HANDLE_DELETER_HPP
