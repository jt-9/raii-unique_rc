#include <catch2/catch_test_macros.hpp>

#include "urc/unique_rc.hpp"
#include "urc/deleter_windows_gdi.hpp"
#include "urc/deleter_windows_kernel.hpp"

#include "testsuite_no_op_deallocator.hpp"

#include <Windows.h>

#include <utility>


//----------------------------------------------------------------
// GDI pen, type HPEN, invalid nullptr
//----------------------------------------------------------------
TEST_CASE("Default initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  constexpr raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t> pen_rc{};

  CHECK(pen_rc.get() == nullptr);
  CHECK_FALSE(pen_rc);
}

TEST_CASE("Value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t> pen_rc{
    CreatePen(PS_SOLID, 1, RGB(0, 0, 0))
  };

  CHECK(pen_rc.get() != nullptr);
  CHECK(pen_rc);
}

TEST_CASE("Release value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t> pen_rc{
    CreatePen(PS_SOLID, 1, RGB(0, 0, 0))
  };

  CHECK(pen_rc.get() != nullptr);
  CHECK(pen_rc);

  decltype(pen_rc) new_pen_rc{ pen_rc.release() };
  CHECK(new_pen_rc.get() != nullptr);
}

TEST_CASE("Reset value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t> pen_rc{
    CreatePen(PS_SOLID, 1, RGB(0, 0, 0))
  };

  CHECK(pen_rc.get() != nullptr);
  CHECK(pen_rc);

  SECTION("Reset with invalid value (nullptr)")
  {
    pen_rc.reset();

    CHECK(pen_rc.get() == nullptr);
    CHECK_FALSE(pen_rc);
  }

  SECTION("Reset with other constructed HPEN")
  {
    const auto non_owner_pen = CreatePen(PS_DASH, 2, RGB(0, 0, 0));
    pen_rc.reset(non_owner_pen);

    CHECK(pen_rc.get() == non_owner_pen);
    CHECK(pen_rc);
  }
}

TEST_CASE("Equality of value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  const raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t>
    pen_rc{ CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) };

  CHECK(pen_rc);

  SECTION("unique_rc::operator == to other value constructed HPEN")
  {
    const raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t>
      pen_rc2{ CreatePen(
      PS_DASH, 4, RGB(0xFF, 0xFF, 0xFF)) };
    CHECK(pen_rc2);

    CHECK_FALSE(pen_rc == pen_rc2);
    CHECK(pen_rc != pen_rc2);
  }

  SECTION("unique_rc::operator == to same, but non-owning unique_rc")
  {
    const raii::unique_rc<HPEN, raii_test::mock_pointer_no_op<HPEN>> noop_pen_rc{ pen_rc.get() };

    CHECK(pen_rc == noop_pen_rc);
    CHECK_FALSE(pen_rc != noop_pen_rc);
  }

  SECTION("unique_rc::operator == to default constructed unique_rc")
  {
    constexpr raii::
      unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t>
        invalid_pen_rc{};
    CHECK_FALSE(invalid_pen_rc);

    CHECK(pen_rc != invalid_pen_rc);
    CHECK(pen_rc != nullptr);

    CHECK(invalid_pen_rc != pen_rc);
    CHECK(nullptr != pen_rc);
  }
}

TEST_CASE("Three-way value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  const raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t>
    pen_rc{ CreatePen(PS_SOLID, 1, RGB(0, 0, 0)) };

  CHECK(pen_rc);

  SECTION("unique_rc::operator <=> with other value constructed unique_rc")
  {
    const raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t>
      pen_rc2{ CreatePen(
      PS_DASH, 4, RGB(0xFF, 0xFF, 0xFF)) };
    CHECK(pen_rc2);

    CHECK((pen_rc <=> pen_rc2) != std::strong_ordering::equal);
  }

  SECTION("unique_rc::operator <=> with same, but non-owning unique_rc")
  {
    const raii::unique_rc<HPEN, raii_test::mock_pointer_no_op<HPEN>> noop_pen_rc{ pen_rc.get() };

    CHECK((pen_rc <=> noop_pen_rc) == std::strong_ordering::equal);
    // CHECK((pen_rc <=> noop_pen_rc) == 0); Catch2 generates error
  }

  SECTION("unique_rc::operator <=> to default constructed unique_rc")
  {
    constexpr raii::
      unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t>
        invalid_pen_rc{};
    CHECK_FALSE(invalid_pen_rc);

    CHECK((pen_rc <=> invalid_pen_rc) == std::strong_ordering::greater);
    CHECK((pen_rc <=> nullptr) == std::strong_ordering::greater);

    CHECK((invalid_pen_rc <=> pen_rc) == std::strong_ordering::less);
    CHECK((nullptr <=> pen_rc) == std::strong_ordering::less);
  }
}

TEST_CASE("Swap value initialised unique_rc<HPEN, gdi_delete_object_nullptr<HPEN>>", "[unique_rc]")
{
  raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t> pen_rc{
    CreatePen(PS_SOLID, 1, RGB(0, 0, 0))
  };

  CHECK(pen_rc);

  SECTION("unique_rc::swap with other value constructed unique_rc")
  {
    raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t> pen_rc2{
      CreatePen(
      PS_DASH, 4, RGB(0xFF, 0xFF, 0xFF)) };
    CHECK(pen_rc2);

    const auto p1 = pen_rc.get();
    const auto p2 = pen_rc2.get();

    pen_rc.swap(pen_rc2);

    CHECK(pen_rc);
    CHECK(pen_rc2);

    CHECK(p2 == pen_rc.get());
    CHECK(p1 == pen_rc2.get());
  }

  SECTION("unique_rc::swap with default constructed unique_rc")
  {
    raii::unique_rc<HPEN, raii::deleter::windows::gdi_delete_object, raii::resolve_handle_type, std::nullptr_t>
      default_init_rc{};
    CHECK_FALSE(default_init_rc);

    const auto p1 = pen_rc.get();
    const auto p2 = default_init_rc.get();

    pen_rc.swap(default_init_rc);

    CHECK(p2 == pen_rc.get());
    CHECK(p1 == default_init_rc.get());
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

  const raii::unique_rc<HANDLE, raii::deleter::windows::close_handle, raii::resolve_handle_type, HANDLE, raii::deleter::windows::invalid_handle_value_policy> pipe{ CreateNamedPipe(pipeName,
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
    const raii::unique_rc<HANDLE,
      raii_test::mock_pointer_no_op<HANDLE>,
      raii::resolve_handle_type,
      HANDLE,
      raii::deleter::windows::invalid_handle_value_policy>
      noop_pipe{ pipe.get() };

    CHECK(pipe == noop_pipe);
    CHECK_FALSE(pipe != noop_pipe);
  }

  SECTION("unique_rc::operator == to default constructed unique_rc")
  {
    const decltype(pipe) default_init_rc{};
    CHECK_FALSE(default_init_rc);

    CHECK(pipe != default_init_rc);
    // CHECK(pipe != nullptr); should not compile, cause invalid is INVALID_HANDLE_VALUE and not nullptr

    CHECK(default_init_rc != pipe);
    CHECK(pipe.invalid() != pipe.get());
  }
}
//****************************************************************
