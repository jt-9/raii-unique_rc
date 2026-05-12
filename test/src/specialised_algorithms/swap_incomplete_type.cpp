#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

#include <concepts>
#include <type_traits>//std::is_swappable_v


namespace {

struct incomplete;

// This function isn't called, we just need to check it compiles.
[[maybe_unused]] void test01(raii::unique_ptr<incomplete> &ptr1, raii::unique_ptr<incomplete> &ptr2)
{
  // PR libstdc++/93562
  ptr1.swap(ptr2);
  std::ranges::swap(ptr1, ptr2);
}

// This function isn't called, we just need to check it compiles.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
[[maybe_unused]] void test02(raii::unique_ptr<incomplete[]> &ptr1, raii::unique_ptr<incomplete[]> &ptr2)
{
  // PR libstdc++/93562
  ptr1.swap(ptr2);
  std::ranges::swap(ptr1, ptr2);
}

namespace A {

struct Deleter
{
  Deleter(const Deleter &) = delete;
  Deleter &operator=(const Deleter &) = delete;

  explicit Deleter(int nid) noexcept : id{ nid } {};

  Deleter(Deleter &&) = default;
  Deleter &operator=(Deleter &&) = delete;

  ~Deleter() = default;

  // [[nodiscard]] static constexpr std::nullptr_t invalid() noexcept { return {}; }

  // [[nodiscard]] static constexpr bool is_owned(int const *ptr) noexcept { return static_cast<bool>(ptr); }

  void operator()(int const *ptr) const noexcept
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete ptr;
  }

  // found by ADL
  friend void swap(Deleter &lhs, Deleter &rhs) noexcept { std::ranges::swap(lhs.id, rhs.id); }

  int id;
};

static_assert(!std::is_move_assignable_v<Deleter>, "not assignable");
static_assert(std::is_swappable_v<Deleter>, "but swappable");
}// namespace A

namespace B {
struct Deleter
{
  Deleter(const Deleter &) = delete;
  Deleter &operator=(const Deleter &) = delete;

  explicit Deleter(int nid) noexcept : id{ nid } {};

  Deleter(Deleter &&) = default;
  Deleter &operator=(Deleter &&) = delete;

  ~Deleter() = default;

  // cppcheck-suppress duplInheritedMember
  // NOLINTNEXTLINE(bugprone-derived-method-shadowing-base-method)
  void operator()(int const *ptr) const noexcept
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete[] ptr;
  }

  // found by ADL
  friend void swap(Deleter &lhs, Deleter &rhs) noexcept { std::ranges::swap(lhs.id, rhs.id); }

  int id;
};
}// namespace B

}// namespace


TEST_CASE("unique_ptr<int> is swappable with non move-assignable deleter", "[unique_ptr][unique_ptr::swap][swap]")
{
  raii::unique_ptr<int, A::Deleter> ptr1{ new int{ 1 }, A::Deleter{ -1 } };
  raii::unique_ptr<int, A::Deleter> ptr2{ new int{ 2 }, A::Deleter{ -2 } };

  int const *const pi1 = ptr1.get();
  int const *const pi2 = ptr2.get();

  // This type must swappable even though the deleter is not move-assignable:
  std::ranges::swap(ptr1, ptr2);

  REQUIRE(ptr1.get() == pi2);
  REQUIRE(ptr1.get_deleter().id == -2);
  REQUIRE(ptr2.get() == pi1);
  REQUIRE(ptr2.get_deleter().id == -1);
}

TEST_CASE("unique_ptr<int[]> is swappable with non move-assignable deleter", "[unique_ptr][unique_ptr::swap][swap]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  raii::unique_ptr<int[], B::Deleter> ptr1{ new int[1]{ 1 }, B::Deleter{ -1 } };
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  raii::unique_ptr<int[], B::Deleter> ptr2{ new int[2]{ 2, 2 }, B::Deleter{ -2 } };

  int const *const pi1 = ptr1.get();
  int const *const pi2 = ptr2.get();

  // This type must swappable even though the deleter is not move-assignable:
  std::ranges::swap(ptr1, ptr2);

  REQUIRE(ptr1.get() == pi2);
  REQUIRE(ptr1.get_deleter().id == -2);
  REQUIRE(ptr2.get() == pi1);
  REQUIRE(ptr2.get_deleter().id == -1);
}