// Defines template class for handles
//

#ifndef RAII_CONCEPTS_HPP
#define RAII_CONCEPTS_HPP

#include "raii_defs.hpp"

#include <concepts>
#include <type_traits>


RAII_NS_BEGIN

template<typename T>
concept has_handle_type = requires { typename T::handle; };

template<typename T>
concept has_pointer_type = requires { typename T::pointer; };

template<typename T>
concept can_form_pointer = requires { std::declval<T *>(); };

template<typename T>
concept is_class_or_union = std::disjunction_v<std::is_class<T>, std::is_union<T>>;

template<typename T>
concept can_reference = requires {
  noexcept(*std::declval<T>());
};

RAII_NS_END

#endif// RAII_CONCEPTS_HPP