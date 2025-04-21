// Defines template class for handles
//

#ifndef RAII_CONCEPTS_HPP
#define RAII_CONCEPTS_HPP

#include "raii_defs.hpp"

#include <concepts>

RAII_NS_BEGIN

template<typename T>
concept has_handle_type = requires { typename T::handle; };

template<typename T>
concept has_pointer_type = requires { typename T::pointer; };

RAII_NS_END

#endif// RAII_CONCEPTS_HPP