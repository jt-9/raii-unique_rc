#ifndef MOCK_POINTER_NO_OP_HPP
#define MOCK_POINTER_NO_OP_HPP

#include <concepts>
#include <cstddef>

namespace mock_raii {

template<typename Handle>
  requires std::is_pointer_v<Handle>
struct mock_pointer_no_op
{
  constexpr mock_pointer_no_op() noexcept = default;

  template<typename U>
  raii_inline constexpr mock_pointer_no_op(const mock_pointer_no_op<U> &) noexcept
    requires std::is_convertible_v<U, Handle>
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(Handle) const noexcept { /* No op */ }
};

}// namespace mock_raii

#endif// MOCK_POINTER_NO_OP_HPP