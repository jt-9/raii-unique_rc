#ifndef WIN_COM_FILE_DLG_HPP
#define WIN_COM_FILE_DLG_HPP

#include "defs.hpp"

#include <concepts>
#include <cstddef>
#include <shobjidl_core.h>

RAII_NS_BEGIN

struct com_object_file_dialog_unadvise
{
  raii_inline constexpr com_object_file_dialog_unadvise(IFileDialog *d = nullptr) noexcept : dlg_{ d } {}

  [[nodiscard]] raii_inline static constexpr DWORD invalid() noexcept { return 0; }

  raii_inline constexpr void operator()(DWORD h) const noexcept
  {
    if (dlg_) { dlg_->Unadvise(h); }
  }

private:
  IFileDialog *dlg_;
};


RAII_NS_END

#endif// WIN_COM_FILE_DLG_HPP
