#include <catch2/catch_test_macros.hpp>

#include "memory_delete.hpp"
#include "testsuite_ptr.hpp"
#include "unique_ptr.hpp"

#include <functional>// std::hash


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
