#include <catch2/catch_test_macros.hpp>

#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

#include <type_traits>

namespace {
template<class T, bool = true> struct nothrow_dereference : std::false_type
{
};

template<class T> struct nothrow_dereference<T, noexcept(*std::declval<T>())> : std::true_type
{
};

// #if __cplusplus >= 202002L// C++20
template<class T>
// NOLINTNEXTLINE(readability-identifier-length)
concept dereferenceable = requires(T &t) { *t; };
// #endif
}// namespace

TEST_CASE("LWG 4324 unique_ptr<void>::operator* is not SFINAE-friendly", "[unique_ptr::operator *]")
{
  // 4324. unique_ptr<void>::operator* is not SFINAE-friendly
  STATIC_REQUIRE(nothrow_dereference<raii::unique_ptr<int>>::value);
  STATIC_REQUIRE_FALSE(nothrow_dereference<raii::unique_ptr<void>>::value);

  // #if __cplusplus >= 202002L// C++20
  STATIC_REQUIRE(dereferenceable<raii::unique_ptr<int>>);
  STATIC_REQUIRE_FALSE(dereferenceable<raii::unique_ptr<void>>);
  // #endif
}