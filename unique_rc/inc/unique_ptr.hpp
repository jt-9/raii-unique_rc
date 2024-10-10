#ifndef UNIQUE_PTR_HPP
#define UNIQUE_PTR_HPP

#include "unique_rc.hpp"

#include "defs.hpp"

#include <cassert>
#include <concepts>

RAII_NS_BEGIN

template<typename Ptr, typename Deleter> 
class unique_ptr : public unique_rc<Ptr *, Deleter>
{
};

RAII_NS_END

#endif// UNIQUE_PTR_HPP