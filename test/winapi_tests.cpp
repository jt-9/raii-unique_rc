#include <catch2/catch_test_macros.hpp>

#include "unique_rc.hpp"
#include "windows_close_handle.hpp"
#include "windows_dc_deleter.hpp"
#include "windows_gdi_deleter.hpp"

#include <utility>
#include <windows.h>


// GDI pen tests. type HPEN
//----------------------------------------------------------------
TEST_CASE("Default initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc::unique_rc]")
{
  constexpr raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{};

  REQUIRE(pen_rc.get() == nullptr);
  REQUIRE_FALSE(pen_rc);
}

TEST_CASE("Value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc::unique_rc]")
{
  raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{ CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) };

  REQUIRE(pen_rc.get() != nullptr);
  REQUIRE(pen_rc);
}
//****************************************************************

// Mutex tests. type HANDLE
//----------------------------------------------------------------
TEST_CASE("Default initialised unique_rc<HANDLE, close_handle_nullptr<HANDLE>>", "[unique_rc::unique_rc]")
{
  constexpr raii::unique_rc<HANDLE, raii::close_handle_nullptr<HANDLE>> mtx{};

  REQUIRE(mtx.get() == nullptr);
  REQUIRE_FALSE(mtx);
}

TEST_CASE("Value initialised unique_rc<HANDLE, close_handle_nullptr<HANDLE>>", "[unique_rc::unique_rc]")
{
  raii::unique_rc<HANDLE, raii::close_handle_nullptr<HANDLE>> mtx{ CreateMutex(nullptr, FALSE, nullptr) };

  REQUIRE(mtx.get() != nullptr);
  REQUIRE(mtx);
}
//****************************************************************