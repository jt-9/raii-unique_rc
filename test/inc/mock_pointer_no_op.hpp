#ifndef MOCK_POINTER_NO_OP_HPP
#define MOCK_POINTER_NO_OP_HPP

#include "mock_generic_deallocator_no_op.hpp"

#include <concepts>
#include <cstddef>

namespace mock_raii {

template<typename Handle>
  requires std::is_pointer_v<Handle>
using mock_pointer_no_op = mock_deallocator_no_op<Handle, std::nullptr_t, nullptr>;


}// namespace mock_raii

#endif// MOCK_POINTER_NO_OP_HPP