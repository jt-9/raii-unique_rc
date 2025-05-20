#include <catch2/catch_test_macros.hpp>

// #include "memory_delete.hpp"
#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

#include <functional>// std::hash
#include <type_traits>

namespace {
// Check for presence/absence of nested types.

template<typename T> using res_type = typename std::hash<T>::result_type;
template<typename T> using arg_type = typename std::hash<T>::argument_type;

template<typename UniqPtr, typename = void, typename... Args> constexpr bool has_res_type(Args &&.../*unused*/) noexcept
{
  return false;
}

template<typename UniqPtr>
constexpr typename std::is_void<res_type<UniqPtr>>::value_type// i.e. bool
  has_res_type() noexcept
{
  return true;
}

template<typename UniqPtr, typename = void, typename... Args> constexpr bool has_arg_type(Args &&.../*unused*/) noexcept
{
  return false;
}

template<typename UniqPtr>
constexpr typename std::is_void<arg_type<UniqPtr>>::value_type// i.e. bool
  has_arg_type()
{
  return true;
}

template<typename UniqPtr> constexpr bool has_no_types()
{
  return !has_res_type<UniqPtr>() && !has_arg_type<UniqPtr>();
}

struct S
{
};

}// namespace

// NOLINTNEXTLINE(cert-dcl58-cpp, cppcoreguidelines-special-member-functions, hicpp-special-member-functions) // disabled specialization for tests
template<> struct std::hash<S *>
{
  hash(hash &&) = delete;
  ~hash() = delete;
};

TEST_CASE("Nested types result_type and argument_type are not present", "[unique_ptr][hash][types]")
{
  STATIC_CHECK(has_no_types<raii::unique_ptr<int>>());
  STATIC_CHECK(has_no_types<raii::unique_ptr<double>>());
}

TEST_CASE("Disabled specializations do not have the nested types", "[unique_ptr][hash][types]")
{
  STATIC_CHECK(has_no_types<raii::unique_ptr<S>>());
}