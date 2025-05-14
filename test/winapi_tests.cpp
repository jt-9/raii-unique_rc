#include <catch2/catch_test_macros.hpp>

#include "unique_rc.hpp"
#include "windows_close_handle.hpp"
#include "windows_dc_delete.hpp"
#include "windows_delete_gdi_object.hpp"

#include "mock_pointer_no_op.hpp"

#include <Windows.h>

#include <utility>


//----------------------------------------------------------------
// GDI pen, type HPEN, invalid nullptr
//----------------------------------------------------------------
TEST_CASE("Default initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  constexpr raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{};

  REQUIRE(pen_rc.get() == nullptr);
  REQUIRE_FALSE(pen_rc);
}

TEST_CASE("Value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{ CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) };

  REQUIRE(pen_rc.get() != nullptr);
  REQUIRE(pen_rc);
}

TEST_CASE("Release value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{ CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) };

  CHECK(pen_rc.get() != nullptr);
  CHECK(pen_rc);

  decltype(pen_rc) new_pen_rc{ pen_rc.release() };
  REQUIRE(new_pen_rc.get() != nullptr);
}

TEST_CASE("Reset value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{ CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) };

  CHECK(pen_rc.get() != nullptr);
  CHECK(pen_rc);

  SECTION("Reset with invalid value (nullptr)")
  {
    pen_rc.reset();

    REQUIRE(pen_rc.get() == nullptr);
    REQUIRE_FALSE(pen_rc);
  }

  SECTION("Reset with other constructed HPEN")
  {
    const auto non_owner_pen = CreatePen(PS_DASH, 2, RGB(0, 0, 0));
    pen_rc.reset(non_owner_pen);

    REQUIRE(pen_rc.get() == non_owner_pen);
    REQUIRE(pen_rc);
  }
}

TEST_CASE("Equality of value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  const raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{ CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) };

  CHECK(pen_rc);

  SECTION("unique_rc::operator == to other value constructed HPEN")
  {
    const raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc2{ CreatePen(
      PS_DASH, 4, RGB(0xFF, 0xFF, 0xFF)) };
    CHECK(pen_rc2);

    REQUIRE_FALSE(pen_rc == pen_rc2);
    REQUIRE(pen_rc != pen_rc2);
  }

  SECTION("unique_rc::operator == to same, but non-owning unique_rc")
  {
    const raii::unique_rc<HPEN, mock_raii::mock_pointer_no_op<HPEN>> noop_pen_rc{ pen_rc.get() };

    REQUIRE(pen_rc == noop_pen_rc);
    REQUIRE_FALSE(pen_rc != noop_pen_rc);
  }

  SECTION("unique_rc::operator == to default constructed unique_rc")
  {
    constexpr raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> invalid_pen_rc{};
    CHECK_FALSE(invalid_pen_rc);

    REQUIRE(pen_rc != invalid_pen_rc);
    REQUIRE(pen_rc != nullptr);

    REQUIRE(invalid_pen_rc != pen_rc);
    REQUIRE(nullptr != pen_rc);
  }
}

TEST_CASE("Three-way value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  const raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{ CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) };

  CHECK(pen_rc);

  SECTION("unique_rc::operator <=> with other value constructed unique_rc")
  {
    const raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc2{ CreatePen(
      PS_DASH, 4, RGB(0xFF, 0xFF, 0xFF)) };
    CHECK(pen_rc2);

    REQUIRE((pen_rc <=> pen_rc2) != std::strong_ordering::equal);
  }

  SECTION("unique_rc::operator <=> with same, but non-owning unique_rc")
  {
    const raii::unique_rc<HPEN, mock_raii::mock_pointer_no_op<HPEN>> noop_pen_rc{ pen_rc.get() };

    REQUIRE((pen_rc <=> noop_pen_rc) == std::strong_ordering::equal);
    // REQUIRE((pen_rc <=> noop_pen_rc) == 0); Catch2 generates error
  }

  SECTION("unique_rc::operator <=> to default constructed unique_rc")
  {
    constexpr raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> invalid_pen_rc{};
    CHECK_FALSE(invalid_pen_rc);

    REQUIRE((pen_rc <=> invalid_pen_rc) == std::strong_ordering::greater);
    REQUIRE((pen_rc <=> nullptr) == std::strong_ordering::greater);

    REQUIRE((invalid_pen_rc <=> pen_rc) == std::strong_ordering::less);
    REQUIRE((nullptr <=> pen_rc) == std::strong_ordering::less);
  }
}

TEST_CASE("Swap value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc{ CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) };

  CHECK(pen_rc);

  SECTION("unique_rc::swap with other value constructed unique_rc")
  {
    raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> pen_rc2{ CreatePen(
      PS_DASH, 4, RGB(0xFF, 0xFF, 0xFF)) };
    CHECK(pen_rc2);

    const auto p1 = pen_rc.get();
    const auto p2 = pen_rc2.get();

    pen_rc.swap(pen_rc2);

    CHECK(pen_rc);
    CHECK(pen_rc2);

    REQUIRE(p2 == pen_rc.get());
    REQUIRE(p1 == pen_rc2.get());
  }

  SECTION("unique_rc::swap with default constructed unique_rc")
  {
    raii::unique_rc<HPEN, raii::gdi_delete_object_nullptr<HPEN>> default_init_rc{};
    CHECK_FALSE(default_init_rc);

    const auto p1 = pen_rc.get();
    const auto p2 = default_init_rc.get();

    pen_rc.swap(default_init_rc);

    REQUIRE(p2 == pen_rc.get());
    REQUIRE(p1 == default_init_rc.get());
  }
}
//****************************************************************


//----------------------------------------------------------------
// Mock CreateFile, type HANDLE, invalid INVALID_HANDLE_VALUE
//----------------------------------------------------------------

TEST_CASE("Equality of value initialised unique_rc<HANDLE, close_handle_invalid_handle_value<>>", "[unique_rc]")
{
  const auto pipeName = TEXT("\\\\.\\pipe\\mytestpipe");
  const auto bufferSize = 8;

  const raii::unique_rc<HANDLE, raii::close_handle_invalid_handle_value<>> pipe{ CreateNamedPipe(pipeName,
    PIPE_ACCESS_DUPLEX,// read/write access
    PIPE_TYPE_MESSAGE |// message type pipe
      PIPE_READMODE_MESSAGE |// message-read mode
      PIPE_WAIT,// blocking mode
    1,// max. instances
    bufferSize,// output buffer size
    bufferSize,// input buffer size
    0,// client time-out
    nullptr) };

  CHECK(pipe);

  SECTION("unique_rc::operator == to same, but non-owning unique_rc")
  {
    const raii::unique_rc<HANDLE, mock_raii::mock_pointer_no_op<HANDLE>> noop_pipe{ pipe.get() };

    REQUIRE(pipe == noop_pipe);
    REQUIRE_FALSE(pipe != noop_pipe);
  }

  SECTION("unique_rc::operator == to default constructed unique_rc")
  {
    const raii::unique_rc<HANDLE, raii::close_handle_invalid_handle_value<>> default_init_rc{};
    CHECK_FALSE(default_init_rc);

    REQUIRE(pipe != default_init_rc);
    // REQUIRE(pipe != nullptr); should not compile, cause invalid is INVALID_HANDLE_VALUE and not nullptr

    REQUIRE(default_init_rc != pipe);
    REQUIRE(pipe.invalid() != pipe.get());
  }
}
//****************************************************************
