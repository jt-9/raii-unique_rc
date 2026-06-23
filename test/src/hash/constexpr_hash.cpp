#include <catch2/catch_test_macros.hpp>

#include "testsuite_ptr.hpp"
#include "urc/unique_ptr.hpp"

#include <functional>// std::hash
#include <cstddef>// std::nullptr_t


namespace {
// User-defined pointer type that throws if a null pointer is dereferenced.
template<typename T> struct Pointer : raii_test::PointerBase<Pointer<T>, T>
{
  using raii_test::PointerBase<Pointer<T>, T>::PointerBase;
};

template<typename T> struct PointerDeleter : raii::default_delete<T>
{
  using pointer = Pointer<T>;

  [[nodiscard]] static constexpr bool is_owned(pointer const &ptr) noexcept;
  void operator()(pointer /*unused*/) const noexcept;
};

template<class T> auto f(int) -> decltype(std::hash<raii::unique_ptr<T, PointerDeleter<T>>>(), std::true_type());

template<class T> auto f(...) -> decltype(std::false_type());
}// namespace


TEST_CASE("std::hash of unique_ptr with custom pointer", "[unique_ptr][hash][constexpr]")
{
  //   std::hash<raii::unique_ptr<Pointer<int>, PointerDeleter<Pointer<int>>>> raii::resolves to
  //   hash_not_enabled
  STATIC_CHECK_FALSE(decltype(f<Pointer<int>>(0))::value);
}


namespace {
template<typename Func, typename Arg, typename = void> struct is_callable : std::false_type
{
};

template<typename Func, typename Arg>
struct is_callable<Func, Arg, decltype((void)(std::declval<Func &>()(std::declval<Arg>())))> : std::true_type
{
};

template<typename Func, typename Arg>
inline constexpr bool is_callable_v = is_callable<Func, Arg>::value;
}// namespace

TEST_CASE("std::hash with empty pointer type", "[unique_ptr][hash]")
{
  struct D
  {
    struct pointer
    {
      // cppcheck-suppress noExplicitConstructor
      // NOLINTNEXTLINE(hicpp-explicit-conversions)
      pointer(std::nullptr_t /*unused*/) {};
    };
    void operator()(pointer /*unused*/) const noexcept {}
  };

  STATIC_CHECK_FALSE(is_callable_v<std::hash<D::pointer> &, D::pointer>);

  using UP = raii::unique_ptr<int, D>;

  // Disabled specializations of hash are not function object types
  STATIC_CHECK_FALSE(is_callable_v<std::hash<UP> &, UP>);
  STATIC_CHECK_FALSE(is_callable_v<std::hash<UP> &, UP &>);
  STATIC_CHECK_FALSE(is_callable_v<std::hash<UP> &, const UP &>);
}