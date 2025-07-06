#include <catch2/catch_test_macros.hpp>

#include "urc/memory_delete.hpp"
#include "urc/unique_rc.hpp"

#include <cassert>
#include <string>
#include <utility>


namespace {
template<typename T>
  requires(!std::is_pointer_v<T>)
constexpr bool ConstexprURcSingle(const T &kInitValue) noexcept
{
  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new)
  raii::unique_rc<T *, raii::memory_delete<T *>> urc1{ new T{ kInitValue } };

  assert(urc1.get() != nullptr);
  assert(urc1);

  decltype(urc1) const urc2{ std::move(urc1) };

  assert(urc2);
  assert(urc2.get() != nullptr);
  assert(*urc2.get() == kInitValue);

  return true;
}
}// namespace

TEST_CASE("Move constructor from initialised unique_rc<char*, memory_delete<char*>>",
  "[unique_rc][unique_rc::unique_rc]")
{
  STATIC_CHECK(ConstexprURcSingle('Q'));
  STATIC_CHECK(ConstexprURcSingle(std::string{ "constexpr urc" }));
}