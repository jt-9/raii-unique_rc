#ifndef WIN_COM_FILE_DLG_HPP
#define WIN_COM_FILE_DLG_HPP

#include "raii_defs.hpp"

#include <Shobjidl.h>

#include <concepts>


RAII_NS_BEGIN

struct com_object_file_dialog_unadvise
{
  struct handle
  {
    IFileDialog *idlg;
    DWORD cookie;


    raii_inline constexpr handle(IFileDialog *dlg, DWORD ncookie) noexcept : idlg{ dlg }, cookie{ ncookie } {}

    raii_inline constexpr handle() noexcept : handle(nullptr, 0) {}

    constexpr handle(const handle &) noexcept = default;
    constexpr handle(handle &&) noexcept = default;

    constexpr handle &operator=(const handle &) noexcept = default;
    constexpr handle &operator=(handle &&) noexcept = default;

    constexpr ~handle() noexcept = default;

    // [[nodiscard]] raii_inline constexpr IFileDialog* operator*() const noexcept { return idlg; }

    [[nodiscard]] friend raii_inline constexpr bool operator==(const handle &lhs, const handle &rhs) noexcept
    {
      return (lhs.cookie == rhs.cookie) && (lhs.idlg == rhs.idlg);
    }

    friend raii_inline constexpr void swap(handle &lhs, handle &rhs) noexcept
    {
      std::ranges::swap(lhs.idlg, rhs.idlg);
      std::ranges::swap(lhs.cookie, rhs.cookie);
    }
  };

  [[nodiscard]] raii_inline static constexpr handle invalid() noexcept { return {}; }

  [[nodiscard]] raii_inline static constexpr bool is_owned(const handle &h) noexcept
  {
    return static_cast<bool>(h.idlg);
  }

  raii_inline void operator()(const handle &h) const noexcept { h.idlg->Unadvise(h.cookie); }
};


RAII_NS_END

#endif// WIN_COM_FILE_DLG_HPP
