#ifndef MOCK_GENERIC_DEALLOCATOR_NO_OP_HPP
#define MOCK_GENERIC_DEALLOCATOR_NO_OP_HPP

#include "raii_defs.hpp"

#include <concepts>

namespace mock_raii {

template<typename Handle, typename InvalidHandle, InvalidHandle invalid_value>
  requires std::is_convertible_v<InvalidHandle, Handle>
struct mock_deallocator_no_op
{
  constexpr mock_deallocator_no_op() noexcept = default;

  template<typename U>
  raii_inline constexpr mock_deallocator_no_op(const mock_deallocator_no_op<U, InvalidHandle, invalid_value> &) noexcept
    requires std::is_convertible_v<U, Handle>
  {}

  [[nodiscard]] raii_inline static constexpr InvalidHandle invalid() noexcept { return invalid_value; }
  [[nodiscard]] raii_inline static constexpr bool is_owned(Handle h) noexcept { return h != invalid(); }

  raii_inline constexpr void operator()(Handle) const noexcept { /* No op */ }
};

}// namespace mock_raii

#endif// MOCK_GENERIC_DEALLOCATOR_NO_OP_HPP