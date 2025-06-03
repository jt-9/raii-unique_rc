#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

#include <cstddef>// std::nullptr_t
#include <type_traits>
#include <utility>


namespace {
struct Derived;

struct Base
{
  Base &operator=(Derived &) = delete;

  [[nodiscard]] static constexpr std::nullptr_t invalid() noexcept { return {}; }

  template<class T> [[nodiscard]] static constexpr bool is_owned(T *ptr) noexcept { return static_cast<bool>(ptr); }

  template<class T> void operator()(T * /*unused*/) const noexcept {}
};

struct Derived : Base
{
};
}// namespace


TEST_CASE("unique_ptr disallowed move assignment", "[unique_ptr][unique_ptr::operator=]")
{
  Base baseDeleter;
  Derived derivedDeleter;

  // NOLINTBEGIN(misc-const-correctness)
  raii::unique_ptr<int, Base &> ub_ptr1{ nullptr, baseDeleter };
  raii::unique_ptr<int, Base> ub_ptr2{ nullptr, baseDeleter };
  raii::unique_ptr<int, Derived &> ud_ptr{ nullptr, derivedDeleter };
  // NOLINTEND(misc-const-correctness)

  // ub_ptr1 = std::move(ud_ptr);// { dg-error "no match" }
  STATIC_CHECK_FALSE(std::is_assignable_v<decltype(ub_ptr1), decltype(std::move(ud_ptr))>);

  // ub_ptr2 = ud_ptr; // { dg-error "no match" }
  // STATIC_CHECK_FALSE(std::is_assignable_v<decltype(ub_ptr2), decltype(ud_ptr)>);

  // NOLINTBEGIN
  raii::unique_ptr<int[], Base &> uba{ nullptr, baseDeleter };
  raii::unique_ptr<int[], Derived &> uda{ nullptr, derivedDeleter };
  // NOLINTEND

  // uba = std::move(uda); // { dg-error "no match" }
  STATIC_CHECK_FALSE(std::is_assignable_v<decltype(uba), decltype(std::move(uda))>);
}