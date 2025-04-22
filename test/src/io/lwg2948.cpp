#include <catch2/catch_test_macros.hpp>

#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

#include <cstddef>//std::nullptr_t
#include <ostream>
#include <sstream>
// #include <utility>//std::declval

namespace {
template<typename S, typename T>
concept streamable = requires(S &ost, const T &ptr) { ost << ptr; };

template<typename T, typename D> bool check(const raii::unique_ptr<T, D> &ptr) noexcept
{
  std::ostringstream ss1;
  ss1 << ptr;

  std::ostringstream ss2;
  ss2 << ptr.get();

  return ss1.str() == ss2.str();
}


template<typename> struct deleter
{
  struct pointer
  {
    constexpr pointer() = default;
    explicit pointer(std::nullptr_t) {}
    explicit operator bool() const { return false; }
    bool operator==([[maybe_unused]] pointer ptr) const { return true; }
  };

  using handle = pointer;

  static constexpr pointer invalid() noexcept {
    return {};
  }

  static constexpr bool is_owned([[maybe_unused]] const pointer& ptr) noexcept {
    return false;
  }

  void operator()([[maybe_unused]] pointer ptr) const {}
};


template<typename C, typename Traits>
// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
int operator<<(std::basic_ostream<C, Traits> &ostream, typename deleter<C>::pointer && /*unused*/)
{
  ostream << C{ 'P' };
  return 1;// no requirement that this operator returns the stream
}

template<typename D>
  using UniquePtr = raii::unique_ptr<typename D::pointer, D>;
}// namespace

TEST_CASE("unique_ptr should define operator <<", "[unique_ptr][operator <<]")
{
  STATIC_REQUIRE(streamable<std::ostream, raii::unique_ptr<int>>);

  raii::unique_ptr<int> ptr;
  REQUIRE(check(ptr));
  ptr = raii::make_unique<int>();
  REQUIRE(check(ptr));
}

TEST_CASE("unique_ptr with deleter should define operator <<", "[unique_ptr][operator <<]")
{
  STATIC_REQUIRE( streamable<std::ostream, UniquePtr<deleter<char>>> );

  const UniquePtr<deleter<char>> ptr;
  REQUIRE( check(ptr) );
}