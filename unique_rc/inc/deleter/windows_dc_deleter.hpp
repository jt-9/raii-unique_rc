#ifndef WIN_GDI_DC_DELETER_HPP
#define WIN_GDI_DC_DELETER_HPP

#include "raii_defs.hpp"

#include <Windows.h>

#include <concepts>


RAII_NS_BEGIN


// Deallocates device context (DC) acquired via CreateDC, CreateCompatibleDC, etc.
// nullptr indicates invalid dc
struct gdi_delete_dc_nullptr
{
  constexpr gdi_delete_dc_nullptr() noexcept = default;

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(HDC h) noexcept { return static_cast<bool>(h); }

  // constexpr generates error - constexpr function doesn't evaluate at compile time
  raii_inline void operator()(HDC h) const noexcept { DeleteDC(h); }
};

// Deallocates device context (DC) acquired via GetDC, GetWindowDC, etc.
// nullptr indicates invalid dc
struct gdi_release_wnd_dc_nullptr
{
  struct handle
  {
    HDC hdc;
    HWND hwnd;


    raii_inline constexpr handle(HDC dc, HWND wnd) noexcept : hdc{ dc }, hwnd{ wnd } {}

    raii_inline constexpr handle() noexcept : handle(nullptr, nullptr) {}

    constexpr handle(const handle &) noexcept = default;
    constexpr handle(handle &&) noexcept = default;

    constexpr handle &operator=(const handle &) noexcept = default;
    constexpr handle &operator=(handle &&) noexcept = default;

    constexpr ~handle() noexcept = default;

    [[nodiscard]] raii_inline constexpr HDC operator*() const noexcept { return hdc; }

    [[nodiscard]] friend raii_inline constexpr bool operator==(const handle &lhs, const handle &rhs) noexcept
    {
      return (lhs.hdc == rhs.hdc) && (lhs.hwnd == rhs.hwnd);
    }

    friend raii_inline constexpr void swap(handle &lhs, handle &rhs) noexcept
    {
      std::ranges::swap(lhs.hdc, rhs.hdc);
      std::ranges::swap(lhs.hwnd, rhs.hwnd);
    }
  };

  // constexpr gdi_release_wnd_dc_nullptr() noexcept = default;

  [[nodiscard]] raii_inline static constexpr handle invalid() noexcept { return {}; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(const handle &h) noexcept
  {
    return static_cast<bool>(h.hdc);
  }

  // constexpr generates error - constexpr function doesn't evaluate at compile time
  raii_inline void operator()(const handle &h) const noexcept { ReleaseDC(h.hwnd, h.hdc); }
};


RAII_NS_END

#endif// WIN_GDI_DC_DELETER_HPP
