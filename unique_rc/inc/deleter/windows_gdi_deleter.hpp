#ifndef WIN_GDI_HANDLE_DELETER_HPP
#define WIN_GDI_HANDLE_DELETER_HPP

#include "raii_defs.hpp"

#include <Windows.h>

#include <concepts>
#include <cstddef>


RAII_NS_BEGIN

// Deallocates GDI objects such as pen, brush, bitmaps, etc.
// nullptr indicates invalid object
template<typename GdiObjeHandle>
  requires std::convertible_to<GdiObjeHandle, HGDIOBJ>
struct gdi_delete_object_nullptr
{
  constexpr gdi_delete_object_nullptr() noexcept = default;

  template<typename U>
  raii_inline constexpr gdi_delete_object_nullptr(const gdi_delete_object_nullptr<U> &) noexcept
    requires std::is_convertible_v<U, GdiObjeHandle>
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return {}; }
  [[nodiscard]] raii_inline static constexpr bool is_owned(GdiObjeHandle h) noexcept { return static_cast<bool>(h); }

  raii_inline constexpr void operator()(GdiObjeHandle h) const noexcept { DeleteObject(h); }
};

RAII_NS_END

#endif// WIN_GDI_HANDLE_DELETER_HPP
