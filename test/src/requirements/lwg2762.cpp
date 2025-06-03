#include <catch2/catch_test_macros.hpp>

#include "urc/deleter/memory_delete.hpp"
#include "urc/unique_ptr.hpp"
#include "urc/unique_rc.hpp"

#include <cstddef>//std::nullptr_t
#include <utility>//std::declval


namespace {
template<bool B> struct TestDeleter
{
  struct pointer
  {
    int &operator*() && noexcept(B);// this is used by unique_ptr
    int &operator*() const & = delete;// this should not be

    int *operator->() noexcept(false);// noexcept here doesn't affect anything

    // Needed for NullablePointer requirements
    explicit pointer(int * = nullptr);

    // cppcheck-suppress noExplicitConstructor intended behaviour
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    pointer(std::nullptr_t);

    bool operator==(const pointer &) const noexcept;
    bool operator!=(const pointer &) const noexcept;
  };

  void operator()([[maybe_unused]] pointer ptr) const noexcept {}
};

template<typename T, bool Nothrow> using UPtr = raii::unique_ptr<T, raii::deleter_class_wrapper<TestDeleter<Nothrow>>>;
}// namespace

TEST_CASE("LWG 2762 unique_ptr operator*() should be noexcept", "[unique_ptr::operator *]")
{
  // 2762. unique_ptr operator*() should be noexcept
  STATIC_CHECK(noexcept(*std::declval<raii::unique_ptr<long>>()));

  // noexcept-specifier depends on the pointer type
  STATIC_CHECK(noexcept(*std::declval<UPtr<int, true> &>()));
  STATIC_CHECK_FALSE(noexcept(*std::declval<UPtr<int, false> &>()));

  // This has always been required, even in C++11.
  STATIC_CHECK(noexcept(std::declval<raii::unique_ptr<long>>().operator->()));
  STATIC_CHECK(noexcept(std::declval<UPtr<int, false> &>().operator->()));
}

TEST_CASE("LWG 2762 unique_rc operator->() should be noexcept", "[unique_rc::operator ->]")
{
  STATIC_CHECK(noexcept(std::declval<raii::unique_rc<long *, raii::default_delete<long>>>().operator->()));
  STATIC_CHECK(
    noexcept(std::declval<raii::unique_rc<int *, raii::deleter_class_wrapper<TestDeleter<false>>> &>().operator->()));
}