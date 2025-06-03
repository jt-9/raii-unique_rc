#ifndef WIN_GDI_DC_DELETE_HPP
#define WIN_GDI_DC_DELETE_HPP

#include "../raii_defs.hpp"

#include <Windows.h>


RAII_NS_BEGIN

// Deallocates device context (DC) acquired via CreateDC, CreateCompatibleDC, etc.
// nullptr indicates invalid dc
struct gdi_delete_dc_nullptr
{
  constexpr gdi_delete_dc_nullptr() noexcept = default;

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(HDC h) noexcept { return static_cast<bool>(h); }

// constexpr generates error - constexpr function doesn't evaluate at compile time
#ifdef __cpp_static_call_operator
  raii_inline static void operator()(HDC h) noexcept
#else
  raii_inline void operator()(HDC h) const noexcept
#endif
  {
    DeleteDC(h);
  }
};

RAII_NS_END

#endif// WIN_GDI_DC_DELETE_HPP
