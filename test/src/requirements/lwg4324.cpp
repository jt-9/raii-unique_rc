#include <catch2/catch_test_macros.hpp>

#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

#include <type_traits>

namespace {

// Excluded because of bug in MSVC compiler
// https://developercommunity.visualstudio.com/t/MSVC-C2753-rejects-noexceptdeclval/11093504
#ifndef _MSC_VER
template<class T, bool = true> struct nothrow_dereference : std::false_type
{
};

template<class T> struct nothrow_dereference<T, noexcept(*std::declval<T>())> : std::true_type
{
};
#endif//_MSC_VER


template<class T>
// NOLINTNEXTLINE(readability-identifier-length)
concept dereferenceable = requires(T &t) { *t; };

}// namespace

TEST_CASE("LWG 4324 unique_ptr<void>::operator* is not SFINAE-friendly", "[unique_ptr::operator *]")
{
  // 4324. unique_ptr<void>::operator* is not SFINAE-friendly

#ifndef _MSC_VER
  STATIC_REQUIRE(nothrow_dereference<raii::unique_ptr<int>>::value);
  STATIC_REQUIRE_FALSE(nothrow_dereference<raii::unique_ptr<void>>::value);
#endif//_MSC_VER

  STATIC_REQUIRE(dereferenceable<raii::unique_ptr<int>>);
  STATIC_REQUIRE(dereferenceable<const raii::unique_ptr<int> &>);

  //NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays)
  STATIC_REQUIRE_FALSE(dereferenceable<raii::unique_ptr<int[]>>);
  STATIC_REQUIRE_FALSE(dereferenceable<const raii::unique_ptr<int[]> &>);
  //NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays)

  STATIC_REQUIRE_FALSE(dereferenceable<raii::unique_ptr<void>>);
  STATIC_REQUIRE_FALSE(dereferenceable<const raii::unique_ptr<void> &>);
}