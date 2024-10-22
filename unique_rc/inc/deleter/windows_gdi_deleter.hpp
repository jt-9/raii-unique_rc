#ifndef WIN_GDI_HANDLE_DELETER_HPP
#define WIN_GDI_HANDLE_DELETER_HPP

#include "raii_defs.hpp"

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

RAII_NS_END

#endif// WIN_GDI_HANDLE_DELETER_HPP
