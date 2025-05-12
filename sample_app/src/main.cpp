// This file will be generated automatically when cur_you run the CMake
// configuration step. It creates a namespace called `myproject`. You can modify
// the source template at `configured_files/config.hpp.in`.
#include "internal_use_only/config.hpp"

// #include "test_deleter.hpp"

#include "consteval_lambda_example.hpp"
#include "coroutine_example.hpp"
#include "test_deleter.hpp"
#include "memory_delete.hpp"
#include "unique_ptr.hpp"
#include "unique_rc.hpp"

#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <CLI/CLI.hpp>

#include <memory>
#include <utility>

#include <concepts>
#include <cstdint>
#include <cstdio>

#include <string_view>
#include <tuple>

// #include <print>
// #include <iostream>

namespace {
void measureAndPrintUniquePtrSize() noexcept
{
  using namespace std::literals;

  // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new)
  const std::unique_ptr<int, raii::memory_delete<int *>> ptr1{ new int(1001) };

  // std::cout << "Sizeof 'std::unique_ptr<int, raii::memory_delete<int*>>' is "sv << sizeof(ptr1) << "bytes\n"sv;
  // std::println("Sizeof 'std::unique_ptr<int, raii::memory_delete<int*>>' is {} bytes"sv, sizeof(ptr1));
  fmt::println("Sizeof 'std::unique_ptr<int, raii::memory_delete<int*>>' is {} bytes"sv, sizeof(ptr1));

  // std::_Compressed_pair<raii::memory_delete<int*>, int*> cp{ std::_Zero_then_variadic_args_t{} };
  // std::cout << "Sizeof 'std::_Compressed_pair<raii::memory_delete<int*>, int*>' is "sv << sizeof(cp) << " bytes\n"sv;

  const std::tuple<int *, raii::memory_delete<int *>> tp_ptr_to_deleter{ nullptr, raii::memory_delete<int *>{} };
  //		std::cout << "Sizeof 'std::tuple<int*, raii::memory_delete<int*>>' is "sv << sizeof(tp) << " bytes\n"sv;
  //		std::println("Sizeof 'std::tuple<int*, raii::memory_delete<int*>>' is {} bytes"sv, sizeof(tp));
  fmt::println("Sizeof 'std::tuple<int*, raii::memory_delete<int*>>' is {} bytes"sv, sizeof(tp_ptr_to_deleter));
}

void printLibVersion(std::string_view lib_name) noexcept
{
  fmt::println("{0} {1}.{2}.{3}",
    lib_name,
    raii::cmake::project_version_major,
    raii::cmake::project_version_minor,
    raii::cmake::project_version_patch);
}

}// namespace


// NOLINTBEGIN(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct SwapTestDel
{
  int m_tag;

  explicit constexpr SwapTestDel(int tag) : m_tag{ tag } {}
  constexpr SwapTestDel(const SwapTestDel &) = delete;
  constexpr SwapTestDel(const SwapTestDel &&src) noexcept : m_tag{ src.m_tag } {};

  constexpr SwapTestDel &operator=(const SwapTestDel &) = delete;
  constexpr SwapTestDel &operator=(SwapTestDel &&rhs) noexcept
  {
    m_tag = rhs.m_tag;

    return *this;
  }

  // static constexpr auto invalid() noexcept { return nullptr; }
  // static constexpr bool is_owned(void const *ptr) noexcept { return ptr != invalid(); }

  void operator()(void * /*unused*/) const {}

  void swap(SwapTestDel &rhs) noexcept
  {
    fmt::println("Member SwapTestDel::swap(this: {}, SwapTestDel &rhs: {}) called", fmt::ptr(this), fmt::ptr(&rhs));

    std::ranges::swap(m_tag, rhs.m_tag);
  }
};
// NOLINTEND(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)

// NOLINTNEXTLINE(misc-use-internal-linkage)
void swap(SwapTestDel &lhs, SwapTestDel &rhs) noexcept
{
  fmt::println("Function swap(SwapTestDel &lhs: {}, SwapTestDel &rhs: {}) called", fmt::ptr(&lhs), fmt::ptr(&rhs));

  lhs.swap(rhs);
}


// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char *argv[]) noexcept
{
  using namespace std::literals;

  CLI::App app{};

  argv = app.ensure_utf8(argv);
  auto *version_flag = app.add_flag("-v,--version", "print raii::unique_rc library version and exit")->ignore_case();

  CLI11_PARSE(app, argc, argv);

  if (*version_flag) {
    printLibVersion("raii::unique_rc"sv);

    return 0;
  }

  raii_sample::testTypeConstructAssignWithConsteval();
  measureAndPrintUniquePtrSize();

  //*/
  using raii::unique_rc;
  
  {
    std::puts("=======================================================");
    const auto kTiedArg1 = -24;
    const char kTiedArg2 = 'A';

    const auto kTiedArg3 = 1724;
    const char kTiedArg4 = 'e';
    // NOLINTBEGIN(bugprone-unhandled-exception-at-new)
    unique_rc<int *, PointerWithDummy2<int *, int, char>> rc1{ new int{ 4 },
      PointerWithDummy2<int *, int, char>{ kTiedArg1, kTiedArg2 } };
    unique_rc<int *, PointerWithDummy2<int *, int, char>> rc2{ new int{ 1 },
      PointerWithDummy2<int *, int, char>{ kTiedArg3, kTiedArg4 } };
    // NOLINTEND(bugprone-unhandled-exception-at-new)

    rc1.swap(rc2);

    rc1 = std::move(rc2);
  }

  {
    std::puts("=======================================================");
    const auto kTiedArg1 = 64;
    const auto kTiedArg2 = 1331;
    // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, readability-isolate-declaration)
    unique_rc<int *, raii::memory_delete<int *>> rc1{ new int{ kTiedArg1 } }, rc2{ new int{ kTiedArg2 } };

    rc1.swap(rc2);

    rc1 = std::move(rc2);

    rc1.reset();
    rc1.reset(rc1.get());

    // std::cout << "Sizeof unique_rc<int*, raii::deleter_class_wrapper<int*> of long long is "sv << sizeof(rc1) << " bytes\n"sv;
    fmt::println("Sizeof unique_rc<int*, raii::deleter_class_wrapper<int*> of long long is {} bytes"sv, sizeof(rc1));
  }

  {
    std::puts("=======================================================");
    const auto kVal1 = -23549;
    const auto kVal2 = 41;
    // NOLINTBEGIN(bugprone-unhandled-exception-at-new)
    unique_rc<int *, raii::memory_delete<int *>> rc1{ new int{ kVal1 } };
    unique_rc<std::int32_t *, raii::memory_delete<std::int32_t *>> rc2{ new std::int32_t{ kVal2 } };
    // NOLINTEND(bugprone-unhandled-exception-at-new)

    rc2 = std::move(rc1);

    fmt::println(
      "After move assignment from unique_rc<int*, raii::deleter_class_wrapper<int*>> to unique_rc<std::int32_t*, "
      "raii::deleter_class_wrapper<std::int32_t*>> {}",
      *rc2.get());
  }

  {
    std::puts("=======================================================");
    fmt::println("unique_rc with default_delete");
    const auto kVal1 = 11;
    // NOLINTBEGIN(bugprone-unhandled-exception-at-new)
    unique_rc<std::int32_t *, raii::default_delete<std::int32_t>> rc1{ new std::int32_t{ kVal1 } };
    // NOLINTEND(bugprone-unhandled-exception-at-new)
    fmt::println("Stored value {}", *rc1.get());

    auto *temp = rc1.release();
    rc1.reset(temp);
  }

  {
    std::puts("=======================================================");
    const auto kVal1 = -23549;
    // NOLINTBEGIN(bugprone-unhandled-exception-at-new)
    raii::unique_ptr<int> ptr1{ new int{ kVal1 } };
    // NOLINTEND(bugprone-unhandled-exception-at-new)

    fmt::println("Value initialised unique_ptr<int, raii::deleter_class_wrapper<int *>> address: {}, value: {}",
      fmt::ptr(ptr1.get()),
      *ptr1.get());

    ptr1.reset();

    if (nullptr == ptr1) { fmt::println("ptr1 is empty"); }
  }

  {
    std::puts("=======================================================");
    const auto kSampleFloat = 3.864F;
    raii::unique_ptr<float> dynamicVal = raii::make_unique<float>(kSampleFloat);
    fmt::println("Value initialised unique_ptr<float> address: {}, value: {}", fmt::ptr(dynamicVal.get()), *dynamicVal);
  }

  {
    std::puts("=======================================================");
    const auto kArraySize = 4;
    // NOLINTBEGIN
    raii::unique_ptr<int[]> arrayUniquePtr = raii::make_unique_for_overwrite<int[]>(kArraySize);
    arrayUniquePtr[0] = -1;
    arrayUniquePtr[1] = 4;
    arrayUniquePtr[2] = 7;

    // cppcheck-suppress leakNoVarFunctionCall false positive
    arrayUniquePtr.reset(new int[2]);
    // NOLINTEND
  }

  {
    std::puts("=======================================================");
    const auto kArraySize = 4;
    // NOLINTBEGIN
    raii::unique_ptr<int[], raii::deleter_class_wrapper<std::default_delete<int[]>>> arrayWithStdDeleter{
      new int[kArraySize]
    };
    arrayWithStdDeleter[0] = -1;
    arrayWithStdDeleter[1] = 4;
    arrayWithStdDeleter[2] = 7;

    // cppcheck-suppress leakNoVarFunctionCall false positive
    arrayWithStdDeleter.reset(new int[2]);
    // NOLINTEND
  }

  {
    std::puts("=======================================================");
    // NOLINTNEXTLINE
    int initA = 2, initB = -7;

    raii::unique_ptr<int, raii::deleter_class_wrapper<SwapTestDel>> ptrA{ &initA,
      raii::deleter_class_wrapper<SwapTestDel>{ initA } };
    raii::unique_ptr<int, raii::deleter_class_wrapper<SwapTestDel>> ptrB{ &initB,
      raii::deleter_class_wrapper<SwapTestDel>{ initB } };

    fmt::println("Before swap ptrA {{v:{}, tag:{}}}, ptrB {{v:{}, tag:{}}}",
      *ptrA,
      ptrA.get_deleter().m_tag,
      *ptrB,
      ptrB.get_deleter().m_tag);

    
    std::ranges::swap(ptrA, ptrB);

    // constexpr auto isDeleterSwappable = std::is_swappable_v<SwapTestDel>;

    fmt::println("After swap ptrA {{v:{}, tag:{}}}, ptrB {{v:{}, tag:{}}}",
      *ptrA,
      ptrA.get_deleter().m_tag,
      *ptrB,
      ptrB.get_deleter().m_tag);
  }
  //*/

  //*/
  {
    std::puts("=======================================================");
    fmt::println("Demonstrating simple coroutine generator...");
    for (const auto rangeElem : raii_sample::range(65, 123)) {
      fmt::print("{:c} ", rangeElem);
      // std::cout << i << ' ';
    }
    // std::cout << '\n';
    fmt::println("");
  }
  //*/
  return 0;
}
