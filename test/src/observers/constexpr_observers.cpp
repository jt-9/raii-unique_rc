#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"

#include <cassert>
#include <cstddef>// std::size_t
#include <utility>// std::as_const


namespace {
template<typename T> [[nodiscard]] constexpr bool unique_ptr_value_array_get(std::size_t array_size) noexcept
{
  // NOLINTNEXTLINE
  auto ptr = raii::unique_ptr<T[]>{ new T[array_size] };

  assert(ptr.get() != nullptr);
  assert(ptr);

  return true;
}

template<typename T>
[[nodiscard]] constexpr bool unique_ptr_value_array_subscript(std::size_t array_size, T first_element) noexcept
{
  // NOLINTNEXTLINE
  auto ptr = raii::unique_ptr<T[]>{ new T[array_size]{} };
  assert(ptr.get() != nullptr);

  ptr[0] = first_element;
  assert(ptr[0] == first_element);

  assert(ptr[array_size - 1] == T{});

  return true;
}

template<typename T> [[nodiscard]] constexpr bool unique_ptr_value_array_get_deleter(std::size_t array_size) noexcept
{
  // NOLINTNEXTLINE
  auto ptr = raii::unique_ptr<T[]>{ new T[array_size] };
  assert(ptr.get() != nullptr);

  [[maybe_unused]] auto &del_ref = ptr.get_deleter();
  [[maybe_unused]] auto &del_ref2 = std::as_const(ptr).get_deleter();

  del_ref(static_cast<T *>(nullptr));

  // auto b1 = static_cast<bool>(p1);
  // assert(b1);

  return true;
}

template<typename T> [[nodiscard]] constexpr bool unique_ptr_value_array_cast_to_bool(std::size_t array_size) noexcept
{
  // NOLINTNEXTLINE
  auto ptr = raii::unique_ptr<T[]>{ new T[array_size] };
  assert(ptr.get() != nullptr);

  const auto owns_memory = static_cast<bool>(ptr);
  assert(owns_memory);

  return true;
}

}// namespace

TEST_CASE("unique_ptr array of ints, unique_ptr::get()", "[unique_ptr][get][constexpr]")
{
  STATIC_CHECK(unique_ptr_value_array_get<int>(5));
}

TEST_CASE("unique_ptr array of ints, unique_ptr::operator[]", "[unique_ptr][subscript operator][constexpr]")
{
  STATIC_CHECK(unique_ptr_value_array_subscript(5, 42));
}

TEST_CASE("unique_ptr array of ints, unique_ptr::get_deleter()", "[unique_ptr][get_deleter][constexpr]")
{
  STATIC_CHECK(unique_ptr_value_array_get_deleter<int>(4));
}

TEST_CASE("unique_ptr array of ints, unique_ptr cast to bool", "[unique_ptr][operator bool][constexpr]")
{
  STATIC_CHECK(unique_ptr_value_array_cast_to_bool<int>(5));
}