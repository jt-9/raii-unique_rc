#ifndef RAII_DELETER_COM_HPP
#define RAII_DELETER_COM_HPP

#include "raii_defs.hpp"

#include <unknwn.h>
// #include <Windows.h>


RAII_NS_BEGIN

namespace deleter {
namespace com {

  template<typename Handle = IUnknown *>
    requires std::is_pointer_v<Handle> && std::derived_from<std::remove_pointer_t<Handle>, IUnknown>
  struct unknown_release
  {
    constexpr unknown_release() noexcept = default;

    template<typename U>
    raii_inline constexpr unknown_release(const unknown_release<U> &) noexcept
      requires std::is_convertible_v<std::remove_pointer_t<U>, std::remove_pointer_t<Handle>>
    {}

#ifdef __cpp_static_call_operator
    raii_inline static void operator()(Handle hnd) noexcept
#else
    raii_inline void operator()(Handle hnd) const noexcept
#endif
    {
      hnd->Release();
    }
  };// unknown_release

}// namespace com
}// namespace deleter

RAII_NS_END

#endif// RAII_DELETER_COM_HPP