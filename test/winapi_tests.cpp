#include <catch2/catch_test_macros.hpp>

#include "unique_rc.hpp"
#include "windows_gdi_deleter.hpp"

#include <utility>
#include <windows.h>

TEST_CASE("Default initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc::unique_rc]")
{
  constexpr raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{};

  REQUIRE(pen_rc.get() == nullptr);
  REQUIRE_FALSE(pen_rc);
  // REQUIRE(factorial(10) == 3628800);
}

TEST_CASE("Value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc::unique_rc]")
{
  raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{ CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) };

  REQUIRE(pen_rc.get() != nullptr);
  REQUIRE(pen_rc);
}