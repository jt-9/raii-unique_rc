#include <catch2/catch_test_macros.hpp>

#include "urc/memory_delete.hpp"
#include "urc/unique_rc.hpp"

#include "testsuite_no_op_deallocator.hpp"

#include <string_view>


TEST_CASE("Equality of default initialised unique_rc<int*, memory_delete<int*>>", "[unique_rc::operator ==]")
{
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc1{};
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc2{};

  SECTION("unique_rc::operator ==(unique_rc, unique_rc)")
  {
    STATIC_CHECK_FALSE(int_rc1);
    STATIC_CHECK_FALSE(int_rc2);
    STATIC_CHECK(int_rc1 == int_rc2);
  }

  SECTION("unique_rc::operator ==(unique_rc, std::nullptr_t)") { STATIC_CHECK(int_rc1 == nullptr); }

  SECTION("unique_rc::operator ==(std::nullptr_t, unique_rc)") { STATIC_CHECK(nullptr == int_rc2); }
}

TEST_CASE("Three-way comparison of default initialised unique_rc<int*, memory_delete<int*>>",
  "[unique_rc::operator <=>]")
{
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc1{};
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc2{};

  SECTION("unique_rc::operator <=>(unique_rc, unique_rc)")
  {
    STATIC_CHECK_FALSE(int_rc1);
    STATIC_CHECK_FALSE(int_rc2);
    STATIC_CHECK((int_rc1 <=> int_rc2) == std::strong_ordering::equal);
  }

  SECTION("unique_rc::operator <=>(unique_rc, std::nullptr_t)")
  {
    STATIC_CHECK((int_rc1 <=> nullptr) == std::strong_ordering::equal);
  }

  SECTION("unique_rc::operator <=>(std::nullptr_t, unique_rc)")
  {
    STATIC_CHECK((nullptr <=> int_rc2) == std::strong_ordering::equal);
  }
}

TEST_CASE("Pointer initialised unique_rc<const char*, memory_mock_delete<const char*>>", "[unique_rc]")
{
  using namespace std::literals;

  constexpr auto str = "This is a test string"sv;

  // NOLINTBEGIN(bugprone-suspicious-stringview-data-usage)
  constexpr raii::unique_rc<const char *, raii_test::mock_pointer_no_op<const char *>> char_rc{ str.data() };
  // NOLINTEND(bugprone-suspicious-stringview-data-usage)

  STATIC_CHECK(char_rc.get() != nullptr);
  STATIC_CHECK(char_rc.get() == str.data());
}

TEST_CASE("Equality of initialised unique_rc<const char*, memory_mock_delete<const char*>>", "[unique_rc::operator ==]")
{
  using namespace std::literals;

  constexpr auto str = "This is a test string"sv;

  // NOLINTBEGIN(bugprone-suspicious-stringview-data-usage)
  constexpr raii::unique_rc<const char *, raii_test::mock_pointer_no_op<const char *>> char_rc1{ str.data() };
  constexpr raii::unique_rc<const char *, raii_test::mock_pointer_no_op<const char *>> char_rc2{ str.data() + 1 };
  // NOLINTEND(bugprone-suspicious-stringview-data-usage)

  SECTION("unique_rc::operator ==(unique_rc, unique_rc)")
  {
    STATIC_CHECK(char_rc1);
    STATIC_CHECK(char_rc2);
    STATIC_CHECK(char_rc1 != char_rc2);
  }

  SECTION("unique_rc::operator ==(unique_rc, std::nullptr_t)") { STATIC_CHECK(char_rc1 != nullptr); }

  SECTION("unique_rc::operator ==(std::nullptr_t, unique_rc)") { STATIC_CHECK(nullptr != char_rc2); }
}

TEST_CASE("Three-way comparison of initialised unique_rc<const char*, memory_mock_delete<const char*>>",
  "[unique_rc::operator <=>]")
{
  using namespace std::literals;

  constexpr auto str = "This is a test string"sv;

  // NOLINTBEGIN(bugprone-suspicious-stringview-data-usage)
  constexpr raii::unique_rc<const char *, raii_test::mock_pointer_no_op<const char *>> char_rc1{ str.data() };
  constexpr raii::unique_rc<const char *, raii_test::mock_pointer_no_op<const char *>> char_rc2{ str.data() + 4 };
  constexpr decltype(char_rc1) copy_rc1{ str.data() };
  // NOLINTEND(bugprone-suspicious-stringview-data-usage)

  STATIC_CHECK(char_rc1);
  STATIC_CHECK(char_rc2);
  STATIC_CHECK((char_rc1 <=> char_rc2) == std::strong_ordering::less);
  STATIC_CHECK((char_rc2 <=> char_rc1) == std::strong_ordering::greater);
  STATIC_CHECK((char_rc1 <=> copy_rc1) == std::strong_ordering::equal);
}
