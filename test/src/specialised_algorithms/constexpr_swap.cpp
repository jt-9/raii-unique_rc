#include <catch2/catch_test_macros.hpp>


#include "memory_delete.hpp"
#include "unique_ptr.hpp"
#include "unique_rc.hpp"

#include <cassert>
#include <concepts>
#include <cstddef>// std::nullptr_t
#include <type_traits>// std::is_swappable_v

namespace {

// NOLINTBEGIN(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct B
{
  static constexpr std::nullptr_t invalid() noexcept;
  static constexpr bool is_owned(void *) noexcept;

  void operator()([[maybe_unused]] void *ptr) const {}
};
// NOLINTEND(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)

void swap(B &, B &) = delete;

// NOLINTBEGIN(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct C
{
  void operator()([[maybe_unused]] void *ptr) const {}
};
// NOLINTEND(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)

void swap(C &, C &) = delete;

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct D
{
  D(D &&) = delete;

  void operator()([[maybe_unused]] void *ptr) const {}
};

struct NoSwapPtr
{
  // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
  struct pointer
  {
    int &operator*() && noexcept;// this is used by unique_ptr
    int &operator*() const & = delete;// this should not be

    int *operator->() noexcept(false);// noexcept here doesn't affect anything

    // Needed for NullablePointer requirements
    explicit pointer(int * = nullptr);
    pointer(pointer &&) = delete;

    bool operator==(const pointer &) const noexcept;
    bool operator!=(const pointer &) const noexcept;
  };

  static constexpr pointer invalid() noexcept;
  static constexpr bool is_owned(pointer) noexcept;

  void operator()([[maybe_unused]] pointer ptr) const noexcept {}
};

}// namespace


TEST_CASE("unique_ptr not swappable via std::swap with deleted swap",
  "[unique_ptr][std::is_swappable][swap][constexpr]")
{
  // Not swappable, and unique_ptr not swappable via the generic std::swap.
  STATIC_CHECK_FALSE(std::is_swappable_v<raii::unique_ptr<int, B>>);
  STATIC_CHECK_FALSE(std::is_swappable_v<raii::unique_ptr<int, raii::deleter_class_wrapper<C>>>);
  STATIC_CHECK_FALSE(std::is_swappable_v<raii::unique_ptr<int, raii::deleter_class_wrapper<D>>>);
}

TEST_CASE("unique_rc not swappable via std::swap with deleted swap", "[unique_rc][std::is_swappable][swap][constexpr]")
{
  // Not swappable, and unique_ptr not swappable via the generic std::swap.
  STATIC_CHECK_FALSE(std::is_swappable_v<raii::unique_rc<int *, B>>);
  STATIC_CHECK_FALSE(std::is_swappable_v<raii::unique_rc<int *, raii::deleter_class_wrapper<C>>>);
  STATIC_CHECK_FALSE(std::is_swappable_v<raii::unique_rc<int *, raii::deleter_class_wrapper<D>>>);

  /* The following code doesn't even compile

  using PtrNoSwapURC = raii::unique_rc<NoSwapPtr::pointer, NoSwapPtr>;
  STATIC_CHECK_FALSE(std::is_swappable_v<PtrNoSwapURC>);

  compile error: constraint (has_static_invalid_convertible_handle) not satisfied for class template
  'unique_rc' because 'has_static_invalid_convertible_handle<std::remove_reference_t<NoSwapPtr>,
  std::decay_t<pointer> >' evaluated to false because type constraint 'std::convertible_to<(anonymous
  namespace)::NoSwapPtr::pointer, (anonymous namespace)::NoSwapPtr::pointer>' was not satisfied
  */

  // constexpr auto isSwappable = std::is_swappable_v<std::unique_ptr<NoSwapPtr::pointer, NoSwapPtr>>; // returns true,
  // but should be false
}

namespace {
template<typename T> [[nodiscard]] constexpr bool swap_unique_ptr_single(T value) noexcept
{
  auto uptr1 = raii::make_unique<T>(value);
  auto uptr2 = raii::make_unique_for_overwrite<T>();

  std::ranges::swap(uptr1, uptr2);

  assert(uptr1);
  assert(uptr2);

  assert(uptr1 == uptr1);
  assert(uptr1 != uptr2);

  assert(value == *uptr2);

  return true;
}

template<typename T>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
[[nodiscard]] constexpr bool swap_unique_ptr_array(std::size_t array1_size, std::size_t array2_size) noexcept
{
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  auto uptr1 = raii::make_unique<T[]>(array1_size);
  auto uptr2 = raii::make_unique_for_overwrite<T[]>(array2_size);
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)

  std::ranges::swap(uptr1, uptr2);

  assert(uptr1);
  assert(uptr2);

  assert(uptr1 == uptr1);
  assert(uptr1 != uptr2);

  return true;
}
}// namespace

TEST_CASE("Swap two unique_ptr of single object", "[unique_ptr][std::is_swappable][swap][constexpr]")
{
  STATIC_CHECK(swap_unique_ptr_single(42));
  STATIC_CHECK(swap_unique_ptr_array<int>(10, 4));
}