#ifndef WIN_GDI_DC_RESTORE_HPP
#define WIN_GDI_DC_RESTORE_HPP

#include "raii_defs.hpp"

#include <Windows.h>

#include <concepts>


RAII_NS_BEGIN


// Restores device context (DC) state obtained with SaveDC
// nullptr indicates invalid dc
struct gdi_restore_dc_nullptr
{
  struct handle
  {
    HDC hdc;
    int state;


    raii_inline constexpr handle(HDC dc, int nstate) noexcept : hdc{ dc }, state{ nstate } {}

    raii_inline constexpr handle() noexcept : handle(nullptr, 0) {}

    constexpr handle(const handle &) noexcept = default;
    constexpr handle(handle &&) noexcept = default;

    constexpr handle &operator=(const handle &) noexcept = default;
    constexpr handle &operator=(handle &&) noexcept = default;

    constexpr ~handle() noexcept = default;

    // [[nodiscard]] raii_inline constexpr HDC operator*() const noexcept { return hdc; }

    [[nodiscard]] friend raii_inline constexpr bool operator==(const handle &lhs, const handle &rhs) noexcept
    {
      return (lhs.state == rhs.state) && (lhs.hdc == rhs.hdc);
    }

    friend raii_inline constexpr void swap(handle &lhs, handle &rhs) noexcept
    {
      std::ranges::swap(lhs.hdc, rhs.hdc);
      std::ranges::swap(lhs.state, rhs.state);
    }
  };

  [[nodiscard]] raii_inline static constexpr handle invalid() noexcept { return {}; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(const handle &h) noexcept
  {
    return static_cast<bool>(h.hdc) && (h.state != 0);
  }

// constexpr generates error - constexpr function doesn't evaluate at compile time
#ifdef __cpp_static_call_operator
  raii_inline static void operator()(const handle &h) noexcept
#else
  raii_inline void operator()(const handle &h) const noexcept
#endif
  {
    RestoreDC(h.hdc, h.state);
  }
};


RAII_NS_END

#endif// WIN_GDI_DC_RESTORE_HPP
