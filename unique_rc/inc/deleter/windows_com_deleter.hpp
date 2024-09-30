// SmartHandle.h : Defines template class for handles
//

#ifndef WIN_COM_HANDLE_DELETER_HPP
#define WIN_COM_HANDLE_DELETER_HPP

#include "defs.hpp"

#include <concepts>
#include <shobjidl_core.h>
#include <cstddef>

RAII_NS_BEGIN

template<typename Handle>
  requires std::is_pointer_v<Handle> && std::derived_from<std::remove_pointer_t<Handle>, IUnknown>
struct com_object_release_nullptr
{
  using handle = std::remove_cv_t<Handle>;

  constexpr com_object_release_nullptr() noexcept = default;

  template<typename U>
  raii_inline constexpr com_object_release_nullptr(const com_object_release_nullptr<U> &) noexcept
    requires std::is_convertible_v<U, handle>
  {}

  [[nodiscard]] raii_inline static constexpr nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(handle h) const noexcept { h->Release(); }
};

struct com_object_file_dialog_unadvise
{
  using handle = DWORD;

  raii_inline constexpr com_object_file_dialog_unadvise(IFileDialog *d = nullptr) noexcept : dlg_{ d } {}

  [[nodiscard]] raii_inline static constexpr handle invalid() noexcept { return 0; }

  raii_inline constexpr void operator()(handle h) const noexcept
  {
    if (dlg_) { dlg_->Unadvise(h); }
  }

private:
  IFileDialog *dlg_;
};

// template< typename Handle >
// using COMHandle = UniqueHandle< COMPolicy< Handle >>;
//
// using COMFileDialogAdviseHandle = UniqueHandle< COMFileDialogAdvisePolicy >;

RAII_NS_END

#endif// WIN_COM_HANDLE_DELETER_HPP
