#include <catch2/catch_test_macros.hpp>


#include "urc/unique_ptr.hpp"
// #include "urc/unique_rc.hpp"

#include <cstddef>
#include <utility>

namespace {
struct D1
{
  [[nodiscard]] static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  [[nodiscard]] static constexpr bool is_owned(int *ptr) noexcept { return static_cast<bool>(ptr); }

  void operator()(int const *ptr) const noexcept
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete ptr;
  }
};

struct D2 : D1
{
  // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
  D2 &operator=(D1 && /*unused*/) noexcept { return *this; }
};

struct DA1
{
  [[nodiscard]] static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  [[nodiscard]] static constexpr bool is_owned(int *ptr) noexcept { return static_cast<bool>(ptr); }

  void operator()(int const *ptr) const noexcept
  {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    delete[] ptr;
  }
};

struct DA2 : DA1
{
  // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
  DA2 &operator=(DA1 && /*unused*/) noexcept { return *this; }
};
}// namespace

TEST_CASE("Move assign single object unique_ptr with deleter base and derived", "[unique_ptr][operator=]")
{
  constexpr auto INIT_VALUE = 28;
  raii::unique_ptr<int, D1> ptr_d1{ new int{ INIT_VALUE } };
  CHECK(*ptr_d1 == INIT_VALUE);

  raii::unique_ptr<int, D2> ptr_d2;
  ptr_d2 = std::move(ptr_d1);
  CHECK(*ptr_d2 == INIT_VALUE);
}

TEST_CASE("Move assign array objects unique_ptr with deleter base and derived", "[unique_ptr][operator=]")
{
  constexpr auto INIT_VALUE = 28;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  raii::unique_ptr<int[], DA1> ptr_da1{ new int[]{ INIT_VALUE } };
  CHECK(ptr_da1[0] == INIT_VALUE);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  raii::unique_ptr<int[], DA2> ptr_da2;
  ptr_da2 = std::move(ptr_da1);
  CHECK(ptr_da2[0] == INIT_VALUE);
}