#include <catch2/catch_test_macros.hpp>

#include "memory_delete.hpp"
#include "unique_ptr.hpp"
#include "unique_rc.hpp"

#include "mock_pointer_no_op.hpp"

#include <string_view>
#include <utility>

TEST_CASE("Default initialised unique_rc<char*, memory_delete<char*>>", "[unique_rc]")
{
  constexpr raii::unique_rc<char *, raii::memory_delete<char *>> char_rc{};

  SECTION("unique_rc::unique_rc()")
  {
    STATIC_REQUIRE(char_rc.get() == nullptr);
    STATIC_REQUIRE_FALSE(char_rc);
    STATIC_REQUIRE(char_rc == nullptr);
  }

  SECTION("unique_rc::invalid()") { STATIC_REQUIRE(decltype(char_rc)::invalid() == nullptr); }
}

TEST_CASE("Equality of default initialised unique_rc<int*, memory_delete<int*>>", "[unique_rc::operator ==]")
{
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc1{};
  constexpr raii::unique_rc<int *, raii::memory_delete<int *>> int_rc2{};

  SECTION("unique_rc::operator ==(unique_rc, unique_rc)")
  {
    STATIC_CHECK_FALSE(int_rc1);
    STATIC_CHECK_FALSE(int_rc2);
    STATIC_REQUIRE(int_rc1 == int_rc2);
  }

  SECTION("unique_rc::operator ==(unique_rc, std::nullptr_t)") { STATIC_REQUIRE(int_rc1 == nullptr); }

  SECTION("unique_rc::operator ==(std::nullptr_t, unique_rc)") { STATIC_REQUIRE(nullptr == int_rc2); }
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
    STATIC_REQUIRE((int_rc1 <=> int_rc2) == std::strong_ordering::equal);
  }

  SECTION("unique_rc::operator <=>(unique_rc, std::nullptr_t)")
  {
    STATIC_REQUIRE((int_rc1 <=> nullptr) == std::strong_ordering::equal);
  }

  SECTION("unique_rc::operator <=>(std::nullptr_t, unique_rc)")
  {
    STATIC_REQUIRE((nullptr <=> int_rc2) == std::strong_ordering::equal);
  }
}

TEST_CASE("Pointer initialised unique_rc<const char*, memory_mock_delete<const char*>>", "[unique_rc]")
{
  using namespace std::literals;

  constexpr auto str = "This is a test string"sv;

  // NOLINTBEGIN(bugprone-suspicious-stringview-data-usage)
  constexpr raii::unique_rc<const char *, mock_raii::mock_pointer_no_op<const char *>> char_rc{ str.data() };
  // NOLINTEND(bugprone-suspicious-stringview-data-usage)

  STATIC_REQUIRE(char_rc.get() != nullptr);
  STATIC_REQUIRE(char_rc.get() == str.data());
}

TEST_CASE("Equality of initialised unique_rc<const char*, memory_mock_delete<const char*>>", "[unique_rc::operator ==]")
{
  using namespace std::literals;

  constexpr auto str = "This is a test string"sv;

  // NOLINTBEGIN(bugprone-suspicious-stringview-data-usage)
  constexpr raii::unique_rc<const char *, mock_raii::mock_pointer_no_op<const char *>> char_rc1{ str.data() };
  constexpr raii::unique_rc<const char *, mock_raii::mock_pointer_no_op<const char *>> char_rc2{ str.data() + 1 };
  // NOLINTEND(bugprone-suspicious-stringview-data-usage)

  SECTION("unique_rc::operator ==(unique_rc, unique_rc)")
  {
    STATIC_CHECK(char_rc1);
    STATIC_CHECK(char_rc2);
    STATIC_REQUIRE(char_rc1 != char_rc2);
  }

  SECTION("unique_rc::operator ==(unique_rc, std::nullptr_t)") { STATIC_REQUIRE(char_rc1 != nullptr); }

  SECTION("unique_rc::operator ==(std::nullptr_t, unique_rc)") { STATIC_REQUIRE(nullptr != char_rc2); }
}

TEST_CASE("Three-way comparison of initialised unique_rc<const char*, memory_mock_delete<const char*>>",
  "[unique_rc::operator <=>]")
{
  using namespace std::literals;

  constexpr auto str = "This is a test string"sv;

  // NOLINTBEGIN(bugprone-suspicious-stringview-data-usage)
  constexpr raii::unique_rc<const char *, mock_raii::mock_pointer_no_op<const char *>> char_rc1{ str.data() };
  constexpr raii::unique_rc<const char *, mock_raii::mock_pointer_no_op<const char *>> char_rc2{ str.data() + 4 };
  constexpr decltype(char_rc1) copy_rc1{ str.data() };
  // NOLINTEND(bugprone-suspicious-stringview-data-usage)

  SECTION("unique_rc::operator <=>(unique_rc, unique_rc)")
  {
    STATIC_CHECK(char_rc1);
    STATIC_CHECK(char_rc2);
    STATIC_REQUIRE((char_rc1 <=> char_rc2) == std::strong_ordering::less);
    STATIC_REQUIRE((char_rc2 <=> char_rc1) == std::strong_ordering::greater);
    STATIC_REQUIRE((char_rc1 <=> copy_rc1) == std::strong_ordering::equal);
  }
}

template<bool B> struct TestDeleter
{
  struct pointer
  {
    int &operator*() && noexcept(B);// this is used by unique_ptr
    int &operator*() const & = delete;// this should not be

    int *operator->() noexcept(false);// noexcept here doesn't affect anything

    // Needed for NullablePointer requirements
    explicit pointer(int * = nullptr);
    bool operator==(const pointer &) const noexcept;
    bool operator!=(const pointer &) const noexcept;
  };

  void operator()([[maybe_unused]] pointer ptr) const noexcept {}
};

template<typename T, bool Nothrow> using UPtr = raii::unique_ptr<T, raii::deleter_wrapper<TestDeleter<Nothrow>>>;

TEST_CASE("LWG  2762 unique_ptr operator*() should be noexcept", "raii::unique_ptr LWG 2762")
{
  // 2762. unique_ptr operator*() should be noexcept
  STATIC_CHECK(noexcept(*std::declval<raii::unique_ptr<long>>()));

  // noexcept-specifier depends on the pointer type
  STATIC_CHECK(noexcept(*std::declval<UPtr<int, true> &>()));
  STATIC_CHECK_FALSE(noexcept(*std::declval<UPtr<int, false> &>()));

  // This has always been required, even in C++11.
  STATIC_REQUIRE(noexcept(std::declval<raii::unique_ptr<long>>().operator->()));
  STATIC_REQUIRE(noexcept(std::declval<UPtr<int, false> &>().operator->()));
}