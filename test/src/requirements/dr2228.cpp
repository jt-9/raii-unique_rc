#include <catch2/catch_test_macros.hpp>

#include "urc/unique_ptr.hpp"
#include "urc/unique_rc.hpp"

#include <type_traits>


struct do_nothing
{
  template<class T> void operator()(T * /*unused*/) const noexcept {}
};

TEST_CASE("DR 2228 is not assignable unique_rc", "[unique_rc::operator=]")
{
  int initValue = 0;
  const raii::unique_rc<int *, do_nothing> uptr1{ &initValue };
  const raii::unique_rc<int *, raii::default_delete<int>> uptr2{};

  STATIC_CHECK_FALSE(std::is_assignable_v<decltype(uptr2), decltype(uptr1)>);
}

TEST_CASE("DR 2228 is not assignable unique_ptr", "[unique_ptr::operator=]")
{
  int initValue = 0;
  const raii::unique_ptr<int, do_nothing> uptr1{ &initValue };
  const raii::unique_ptr<int> uptr2{};

  STATIC_CHECK_FALSE(std::is_assignable_v<decltype(uptr2), decltype(uptr1)>);
}