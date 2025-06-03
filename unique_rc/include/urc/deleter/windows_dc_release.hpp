#ifndef WIN_GDI_DC_RELEASE_HPP
#define WIN_GDI_DC_RELEASE_HPP

#include "../raii_defs.hpp"

#include <Windows.h>

#include <concepts>// std::ranges::swap


RAII_NS_BEGIN

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
#ifdef __cpp_static_call_operator
  raii_inline static void operator()(const handle &h) noexcept
#else
  raii_inline void operator()(const handle &h) const noexcept
#endif
  {
    ReleaseDC(h.hwnd, h.hdc);
  }
};

RAII_NS_END

#endif// WIN_GDI_DC_DELETER_HPP
