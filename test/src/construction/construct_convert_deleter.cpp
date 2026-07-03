#include <catch2/catch_test_macros.hpp>

#include "urc/memory_free.hpp"
#include "urc/unique_ptr.hpp"

#include <type_traits>// std::is_[default]_constructible_v

namespace {
template<typename T, typename D, typename... Args> constexpr bool unique_ptr_is_constructible() noexcept
{ return std::is_constructible_v<raii::unique_ptr<T, D>, Args...>; }

struct Base
{
  int m_int = 0;
};

struct Derived : Base
{
};
}// namespace

TEST_CASE("unique_ptr is constructible with deleter of type convertible to pointer type",
  "[unique_ptr][std::is_constructible_v]")
{
  STATIC_CHECK(unique_ptr_is_constructible<void, raii::default_delete<void>, int *, raii::default_delete<int>>());
  STATIC_CHECK(
    unique_ptr_is_constructible<Base, raii::default_delete<Base>, Derived *, raii::default_delete<Derived>>());
}

TEST_CASE("memory_free of type T is constructible from memory_free of type U, if U is convertible to T",
  "[deleter][std::is_constructible_v]")
{
  STATIC_CHECK(std::is_constructible_v<raii::memory_free<void *>, raii::memory_free<float *>>);
  STATIC_CHECK(std::is_constructible_v<raii::memory_free<Base *>, raii::memory_free<Derived *>>);
}