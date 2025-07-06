#include <catch2/catch_test_macros.hpp>

// #include "urc/unique_rc.hpp"
#include "urc/unique_ptr.hpp"


namespace {
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
int delete_call_count = 0;

constexpr void reset_counter() noexcept { delete_call_count = 0; }

constexpr void del(const int *ptr) noexcept
{
  ++delete_call_count;

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete ptr;
}

constexpr void vdel(const int *ptr) noexcept
{
  ++delete_call_count;

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] ptr;
}

}// namespace


TEST_CASE("unique_ptr of single object constructed via new and deleter as pointer to function",
  "[unique_ptr][unique_ptr::unique_ptr]")
{
  reset_counter();
  {
    const raii::unique_ptr<int, void (*)(const int *)> null_uptr{ nullptr, del };
  }
  CHECK(delete_call_count == 0);

  {
    const raii::unique_ptr<int, void (*)(const int *)> uptr{ new int, del };
  }
  CHECK(delete_call_count == 1);
}

TEST_CASE("unique_ptr of array of objects constructed via new[] and deleter as pointer to function",
  "[unique_ptr][unique_ptr::unique_ptr]")
{
  reset_counter();
  {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
    const raii::unique_ptr<int[], void (*)(const int *)> null_uvptr{ nullptr, vdel };
  }
  CHECK(delete_call_count == 0);

  {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
    const raii::unique_ptr<int[], void (*)(const int *)> uvptr{ new int[1], vdel };
  }
  CHECK(delete_call_count == 1);
}