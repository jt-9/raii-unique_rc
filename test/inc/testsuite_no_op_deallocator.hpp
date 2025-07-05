#ifndef RAII_TESTSUITE_NO_OP_DEALLOCATOR_HPP
#define RAII_TESTSUITE_NO_OP_DEALLOCATOR_HPP

#include "urc/raii_defs.hpp"

#include <concepts>
#include <cstddef>// std::nullptr_t


namespace raii_test {

template<typename Handle>
struct mock_deallocator_no_op
{
  constexpr mock_deallocator_no_op() noexcept = default;

  template<typename U>
  raii_inline constexpr mock_deallocator_no_op(const mock_deallocator_no_op<U> &) noexcept
    requires std::is_convertible_v<U, Handle>
  {}

  raii_inline constexpr void operator()(Handle) const noexcept { /* No op */ }
};


template<typename Ptr>
  requires std::is_pointer_v<Ptr>
using mock_pointer_no_op = mock_deallocator_no_op<Ptr>;

}// namespace raii_test

#endif// RAII_TESTSUITE_NO_OP_DEALLOCATOR_HPP