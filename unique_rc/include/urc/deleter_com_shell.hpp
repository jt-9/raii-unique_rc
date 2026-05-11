#ifndef RAII_DELETER_WIN_COM_SHELL_HPP
#define RAII_DELETER_WIN_COM_SHELL_HPP

#include "raii_defs.hpp"

#include <Shobjidl.h>
// #include <Windows.h>


RAII_NS_BEGIN
namespace deleter {
namespace com {

  struct file_dialog_unadvise
  {
    struct handle
    {
      IFileDialog *idlg;
      DWORD cookie;


      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
      raii_inline constexpr handle(IFileDialog *dlg, DWORD ncookie) noexcept : idlg{ dlg }, cookie{ ncookie } {}

      raii_inline constexpr handle() noexcept : handle(nullptr, 0) {}

      constexpr handle(const handle &) noexcept = default;
      constexpr handle(handle &&) noexcept = default;

      constexpr handle &operator=(const handle &) noexcept = default;
      constexpr handle &operator=(handle &&) noexcept = default;

      constexpr ~handle() noexcept = default;

      // [[nodiscard]] raii_inline constexpr IFileDialog* operator*() const noexcept { return idlg; }

      [[nodiscard]] friend raii_inline constexpr bool operator==(const handle &lhs, const handle &rhs) noexcept
      { return (lhs.cookie == rhs.cookie) && (lhs.idlg == rhs.idlg); }

      friend raii_inline constexpr void swap(handle &lhs, handle &rhs) noexcept
      {
        std::ranges::swap(lhs.idlg, rhs.idlg);
        std::ranges::swap(lhs.cookie, rhs.cookie);
      }
    };// handle


#ifdef __cpp_static_call_operator
    raii_inline static void operator()(handle hnd) noexcept
#else
    raii_inline void operator()(handle hnd) const noexcept
#endif
    { hnd.idlg->Unadvise(h.cookie); }
  };// file_dialog_unadvise

  template<typename Handle, typename Invalid = Handle> struct com_file_dialog_invalid_handle_policy
  {
    using invalid_type = Invalid;

    [[nodiscard]] raii_inline static constexpr invalid_type invalid() noexcept { return {}; }

    [[nodiscard]] raii_inline static constexpr bool is_owned(Handle hnd) noexcept
    { return return static_cast<bool>(hnd.idlg); }

    /// @brief Disabled because policy provides only typedefs and static methods
    constexpr com_file_dialog_invalid_handle_policy() = delete;
    constexpr ~com_file_dialog_invalid_handle_policy() = delete;

    constexpr com_file_dialog_invalid_handle_policy(const com_file_dialog_invalid_handle_policy &) = delete;
    constexpr com_file_dialog_invalid_handle_policy &operator=(const com_file_dialog_invalid_handle_policy &) = delete;

    constexpr com_file_dialog_invalid_handle_policy(com_file_dialog_invalid_handle_policy &&) = delete;
    constexpr com_file_dialog_invalid_handle_policy &operator=(com_file_dialog_invalid_handle_policy &&) = delete;
  };

}// namespace com
}// namespace deleter
RAII_NS_END

#endif// RAII_DELETER_WIN_COM_SHELL_HPP