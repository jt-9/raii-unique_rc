#ifndef MEMORY_MOCK_DELETER_HPP
#define MEMORY_MOCK_DELETER_HPP

#include <concepts>
#include <cstddef>

namespace mock_raii {

template<typename Handle>
  requires std::is_pointer_v<Handle>
struct memor_mock_delete
{
  constexpr memor_mock_delete() noexcept = default;

  template<typename U>
  raii_inline constexpr memor_mock_delete(const memor_mock_delete<U> &) noexcept
    requires std::is_convertible_v<U, Handle>
  {}

  [[nodiscard]] raii_inline static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  raii_inline constexpr void operator()(Handle) const noexcept
  {
    // No op
  }
};

}// namespace mock_raii

#endif// MEMORY_MOCK_DELETER_HPP