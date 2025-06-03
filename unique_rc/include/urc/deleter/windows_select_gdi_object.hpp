#ifndef WIN_GDI_SELECT_OBJECT_HPP
#define WIN_GDI_SELECT_OBJECT_HPP

#include "../raii_defs.hpp"

#include <Windows.h>

#include <concepts>// std::convertible_to
#include <cstddef>// std::nullptr_t


RAII_NS_BEGIN

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
    {
      return (lhs.hobj == rhs.hobj) && (lhs.hdc == rhs.hdc);
    }

    friend raii_inline constexpr void swap(handle &lhs, handle &rhs) noexcept
    {
      std::ranges::swap(lhs.hdc, rhs.hdc);
      std::ranges::swap(lhs.hobj, rhs.hobj);
    }
  };

  [[nodiscard]] raii_inline static constexpr handle invalid() noexcept { return {}; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(const handle &h) noexcept
  {
    return static_cast<bool>(h.hobj);
  }

#ifdef __cpp_static_call_operator
  raii_inline static void operator()(const handle &h) noexcept
#else
  raii_inline void operator()(const handle &h) const noexcept
#endif
  {
    SelectObject(h.hdc, h.hobj);
  }
};

RAII_NS_END

#endif// WIN_GDI_SELECT_OBJECT_HPP
