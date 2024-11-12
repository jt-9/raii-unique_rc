#ifndef WIN_GDI_DC_RESTORE_HPP
#define WIN_GDI_DC_RESTORE_HPP

#include "raii_defs.hpp"

#include <concepts>
#include <cstddef>

#include <Windows.h>


RAII_NS_BEGIN


// Restores device context (DC) state obtained with SaveDC
// nullptr indicates invalid dc
struct gdi_restore_dc_nullptr
{
  raii_inline explicit constexpr gdi_restore_dc_nullptr(int state) noexcept : state_{ state } {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  // constexpr generates error - constexpr function doesn't evaluate at compile time
  raii_inline void operator()(HDC h) const noexcept { RestoreDC(h, state_); }

private:
  int state_;
};


RAII_NS_END

#endif// WIN_GDI_DC_RESTORE_HPP
