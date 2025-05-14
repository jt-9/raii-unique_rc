#ifndef WIN_GDI_DELETE_HANDLE_HPP
#define WIN_GDI_DELETE_HANDLE_HPP

#include "raii_defs.hpp"

#include <Windows.h>

#include <concepts>// std::convertible_to
#include <cstddef>// std::nullptr_t


RAII_NS_BEGIN

// Deallocates GDI objects such as pen, brush, bitmaps, etc.
// nullptr indicates invalid object
template<typename GdiObjHandle>
  requires std::convertible_to<GdiObjHandle, HGDIOBJ>
struct gdi_delete_object_nullptr
{
  constexpr gdi_delete_object_nullptr() noexcept = default;

  template<typename U>
  raii_inline constexpr gdi_delete_object_nullptr(const gdi_delete_object_nullptr<U> &) noexcept
    requires std::is_convertible_v<U, GdiObjHandle>
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return {}; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(GdiObjHandle h) noexcept { return static_cast<bool>(h); }

#ifdef __cpp_static_call_operator
  raii_inline static void operator()(GdiObjHandle h) noexcept
#else
  raii_inline void operator()(GdiObjHandle h) const noexcept
#endif
  {
    DeleteObject(h);
  }
};

RAII_NS_END

#endif// WIN_GDI_DELETE_HANDLE_HPP
