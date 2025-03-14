#ifndef WIN_GDI_SELECT_OBJECT_HPP
#define WIN_GDI_SELECT_OBJECT_HPP

#include "raii_defs.hpp"

#include <concepts>
#include <cstddef>
#include <utility>

#include <Windows.h>


RAII_NS_BEGIN

// Selects GDI objects such as pen, brush, bitmap, etc.
// nullptr indicates invalid object
template<typename Handle>
  requires std::convertible_to<Handle, HGDIOBJ>
struct gdi_select_object_nullptr
{
  raii_inline explicit constexpr gdi_select_object_nullptr(HDC hdc) noexcept : hdc_{ hdc } {}

  template<typename U>
  raii_inline constexpr gdi_select_object_nullptr(const gdi_select_object_nullptr<U> &src) noexcept
    requires std::is_convertible_v<U, Handle>
    : hdc_{ src.hdc_ }
  {}

  template<typename U>
  raii_inline constexpr gdi_select_object_nullptr(gdi_select_object_nullptr<U> &&src) noexcept
    requires std::is_convertible_v<U, Handle>
    : hdc_{ std::exchange(src.hdc_, nullptr) }
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }
  [[nodiscard]] raii_inline static constexpr bool is_valid(Handle h) noexcept { return h; }

  raii_inline constexpr void operator()(Handle h) const noexcept { SelectObject(hdc_, h); }

private:
  HDC hdc_;
};

RAII_NS_END

#endif// WIN_GDI_SELECT_OBJECT_HPP
