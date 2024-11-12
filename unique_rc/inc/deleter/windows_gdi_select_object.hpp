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
    : hdc_{ std::move(src.release_hdc()) }
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(Handle h) const noexcept { SelectObject(hdc_, h); }

private:
  raii_inline constexpr HDC release_hdc() const noexcept
  {
    const auto old_hdc = hdc_;
    hdc_ = nullptr;

    return old_hdc;
  }

private:
  HDC hdc_;
};

RAII_NS_END

#endif// WIN_GDI_SELECT_OBJECT_HPP
