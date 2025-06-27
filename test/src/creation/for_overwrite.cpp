#include <catch2/catch_test_macros.hpp>

#include "urc/unique_ptr.hpp"
// #include <memory>

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>// std::memset

namespace {
constexpr auto constFillValue = static_cast<unsigned char>(0xAA);
constexpr std::size_t constArraySize = 4;
constexpr auto constInitMem = 0xBB;
}// namespace

// NOLINTBEGIN(cppcoreguidelines-no-malloc, hicpp-no-malloc, cppcoreguidelines-owning-memory)
void *operator new(std::size_t size)
{
  void *ptr = std::malloc(size);
  assert(ptr != nullptr);

  std::memset(ptr, constFillValue, size);
  return ptr;
}

void *operator new[](std::size_t size)
{
  void *ptr = std::malloc(size);
  std::memset(ptr, constFillValue, size);
  assert(ptr != nullptr);

  return ptr;
}

void operator delete(void *ptr) noexcept { std::free(ptr); }
void operator delete(void *ptr, std::size_t /*unused*/) noexcept { std::free(ptr); }

void operator delete[](void *ptr) noexcept { std::free(ptr); }
void operator delete[](void *ptr, std::size_t /*unused*/) noexcept { std::free(ptr); }

// NOLINTEND(cppcoreguidelines-no-malloc, hicpp-no-malloc, cppcoreguidelines-owning-memory)


TEST_CASE("raii::make_unique_for_overwrite single int via malloc with memset",
  "[unique_ptr][make_unique_for_overwrite]")
{
  raii::unique_ptr<int> ptr = raii::make_unique_for_overwrite<int>();
  CHECK(ptr != nullptr);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  unsigned char buffer[sizeof(int)];
  std::memcpy(static_cast<void *>(buffer), ptr.get(), sizeof(buffer));

  for (const auto elem : buffer) { CHECK(elem == constFillValue); }
}

TEST_CASE("raii::make_unique_for_overwrite array of ints via malloc with memset",
  "[unique_ptr][make_unique_for_overwrite]")
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  raii::unique_ptr<int[]> aptr = raii::make_unique_for_overwrite<int[]>(constArraySize);
  CHECK(aptr != nullptr);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  unsigned char buffer[constArraySize * sizeof(int)];
  std::memcpy(static_cast<void *>(buffer), aptr.get(), sizeof(buffer));

  for (const auto elem : buffer) { CHECK(elem == constFillValue); }
}

TEST_CASE("raii::make_unique_for_overwrite via malloc with memset non-trivial type",
  "[unique_ptr][make_unique_for_overwrite]")
{
  // Type with non-trivial initialization should still be default-initialized.
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init) overloaded new[] initialises raw memory
  struct NonTrivial
  {
    int init = constInitMem;
    int uninit;
  };

  const raii::unique_ptr<NonTrivial> single_ptr = raii::make_unique_for_overwrite<NonTrivial>();
  CHECK(single_ptr->init == constInitMem);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
  const raii::unique_ptr<NonTrivial[]> array_ptr = raii::make_unique_for_overwrite<NonTrivial[]>(constArraySize);
  for (std::size_t i = 0; i < constArraySize; i++) { CHECK(array_ptr[i].init == constInitMem); }
}