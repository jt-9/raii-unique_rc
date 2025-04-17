#include <catch2/catch_test_macros.hpp>

#include "memory_delete.hpp"
#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

#include <cstddef>// std::size_t
#include <functional>
#include <type_traits>

namespace {
template<typename Func, typename Arg, typename = void> struct is_callable : std::false_type
{
};

template<typename Func, typename Arg>
struct is_callable<Func, Arg, decltype((void)(std::declval<Func &>()(std::declval<Arg>())))> : std::true_type
{
};

template<typename Func, typename Arg> constexpr bool is_callable_v = is_callable<Func, Arg>::value;
}// namespace

TEST_CASE("Test hash for unique_ptr{new Empty{}}, and unique_ptr{new Empty[]]}", "[unique_ptr][hash]")
{
  struct Empty
  {
  };

  const raii::unique_ptr<Empty> ptr0{ new Empty{} };
  const std::hash<raii::unique_ptr<Empty>> hu0;
  const std::hash<typename raii::unique_ptr<Empty>::pointer> hp0;

  REQUIRE(hu0(ptr0) == hp0(ptr0.get()));

  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  const raii::unique_ptr<Empty[]> ptr1{ new Empty[10] };
  const std::hash<raii::unique_ptr<Empty[]>> hu1;
  const std::hash<typename raii::unique_ptr<Empty[]>::pointer> hp1;
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)

  REQUIRE(hu1(ptr1) == hp1(ptr1.get()));
}

TEST_CASE("Test hash with empty pointer type", "[unique_ptr][hash]")
{
  struct D
  {
    struct pointer
    {
      // cppcheck-suppress noExplicitConstructor intended behaviour
      // NOLINTNEXTLINE(hicpp-explicit-conversions)
      pointer(std::nullptr_t);
    };
    void operator()([[maybe_unused]] pointer ptr) const noexcept {}
  };

  STATIC_REQUIRE_FALSE(is_callable_v<std::hash<D::pointer> &, D::pointer>);

  using UP = raii::unique_ptr<int, raii::pointer_deleter_wrapper<D>>;

  // Disabled specializations of hash are not function object types
  STATIC_REQUIRE_FALSE(is_callable_v<std::hash<UP> &, UP>);
  STATIC_REQUIRE_FALSE(is_callable_v<std::hash<UP> &, UP &>);
  STATIC_REQUIRE_FALSE(is_callable_v<std::hash<UP> &, const UP &>);
}


namespace {
struct D
{
  struct pointer
  {
    // cppcheck-suppress noExplicitConstructor intended behaviour
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr pointer(std::nullptr_t = nullptr) {};

    bool operator==(std::nullptr_t) const { return false; }
    bool operator!=(std::nullptr_t) const { return true; }
  };

  using handle = pointer;

  [[nodiscard]] static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  [[nodiscard]] static constexpr bool is_owned([[maybe_unused]] pointer ptr) noexcept { return true; }

  void operator()([[maybe_unused]] pointer ptr) const noexcept {}
};

struct F
{
  struct pointer
  {
    bool operator==(std::nullptr_t) const { return false; }
    bool operator!=(std::nullptr_t) const { return true; }
  };

  using handle = pointer;

  [[nodiscard]] static constexpr pointer invalid() noexcept { return {}; }

  [[nodiscard]] static constexpr bool is_owned([[maybe_unused]] pointer ptr) noexcept { return true; }

  void operator()([[maybe_unused]] pointer ptr) const noexcept {}
};

}// namespace

namespace std {
template<> struct hash<D::pointer>
{
  // NOLINTNEXTLINE(hicpp-exception-baseclass)
  std::size_t operator()([[maybe_unused]] D::pointer ptr) const { throw 1; }
};

template<> struct hash<F::pointer>
{
  // NOLINTNEXTLINE(hicpp-exception-baseclass)
  std::size_t operator()([[maybe_unused]] F::pointer ptr) const { throw 3; }
};
}// namespace std

TEST_CASE("Test hash operator() throw", "[unique_ptr][hash][throw]")
{
  using UP = raii::unique_ptr<int, D>;
  const UP ptr1;
  const std::hash<UP> hu1;

  REQUIRE_THROWS_AS(hu1(ptr1), int);

  // Should still be noexcept if the underlying hash object is:
  using UP2 = raii::unique_ptr<int>;
  constexpr UP2 ptr2;
  constexpr std::hash<UP2> hu2;

  STATIC_REQUIRE(noexcept(hu2(ptr2)));


  using UP3 = raii::unique_ptr<int, F>;
  const UP3 ptr3;
  const std::hash<UP3> hu3;

  REQUIRE_THROWS_AS(hu3(ptr3), int);
}
