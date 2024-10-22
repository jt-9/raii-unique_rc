#ifndef WIN_COM_RELEASE_HPP
#define WIN_COM_RELEASE_HPP

#include "raii_defs.hpp"

#include <concepts>
#include <cstddef>

#include <unknwn.h>

RAII_NS_BEGIN

template<typename Handle = IUnknown>
  requires std::is_pointer_v<Handle> && std::derived_from<std::remove_pointer_t<Handle>, IUnknown>
struct com_object_release_nullptr
{
  constexpr com_object_release_nullptr() noexcept = default;

  template<typename U>
  raii_inline constexpr com_object_release_nullptr(const com_object_release_nullptr<U> &) noexcept
    requires std::is_convertible_v<U, Handle>
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  raii_inline void operator()(Handle h) const noexcept { h->Release(); }
};

RAII_NS_END

#endif// WIN_COM_RELEASE_HPP
