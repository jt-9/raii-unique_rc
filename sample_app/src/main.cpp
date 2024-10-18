// This file will be generated automatically when cur_you run the CMake
// configuration step. It creates a namespace called `myproject`. You can modify
// the source template at `configured_files/config.hpp.in`.
#include "internal_use_only/config.hpp"

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

#include <cstdint>
// #include <string>
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

  measureAndPrintUniquePtrSize();

  using namespace raii;

  {
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
    const auto kTiedArg1 = 64;
    const auto kTiedArg2 = 1331;
    // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, readability-isolate-declaration)
    unique_rc<int *, memory_delete<int *>> rc1{ new int{ kTiedArg1 } }, rc2{ new int{ kTiedArg2 } };

    rc1.swap(rc2);

    rc1 = std::move(rc2);

    rc1.reset();
    rc1.reset(rc1.get());

    // std::cout << "Sizeof unique_rc<int*, memory_delete<int*> of long long is "sv << sizeof(rc1) << " bytes\n"sv;
    fmt::println("Sizeof unique_rc<int*, memory_delete<int*> of long long is {} bytes"sv, sizeof(rc1));
  }

  {
    const auto kVal1 = -23549;
    const auto kVal2 = 41;
    // NOLINTBEGIN(bugprone-unhandled-exception-at-new)
    unique_rc<int *, raii::memory_delete<int *>> rc1{ new int{ kVal1 } };
    unique_rc<std::int32_t *, raii::memory_delete<std::int32_t *>> rc2{ new std::int32_t{ kVal2 } };
    // NOLINTEND(bugprone-unhandled-exception-at-new)

    rc2 = std::move(rc1);

    fmt::println(
      "After move assignment from unique_rc<int*, raii::memory_delete<int*>> to unique_rc<std::int32_t*, "
      "raii::memory_delete<std::int32_t*>> {}",
      *rc2.get());
  }

  {
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
    const auto kVal1 = -23549;
    // NOLINTBEGIN(bugprone-unhandled-exception-at-new)
    raii::unique_ptr<int> ptr1{ new int{ kVal1 } };
    // NOLINTEND(bugprone-unhandled-exception-at-new)

    fmt::println("Value initialised unique_ptr<int, raii::memory_delete<int *>> address: {}, value: {}",
      fmt::ptr(ptr1.get()),
      *ptr1.get());

    ptr1.reset();

    if (nullptr == ptr1) { fmt::println("ptr1 is empty"); }
  }

  {
    const auto kSampleFloat = 3.864F;
    raii::unique_ptr<float> dynamicVal = raii::make_unique<float>(kSampleFloat);
    fmt::println("Value initialised unique_ptr<float> address: {}, value: {}", fmt::ptr(dynamicVal.get()), *dynamicVal);
  }

  {
    const auto kArraySize = 4;
    // NOLINTBEGIN
    raii::unique_ptr<int[]> arrayUniquePtr = raii::make_unique_for_overwrite<int[]>(kArraySize);
    arrayUniquePtr[0] = -1;
    arrayUniquePtr[1] = 4;
    arrayUniquePtr[2] = 7;

    arrayUniquePtr.reset(new int[2]);
    // NOLINTEND
  }

  {
    const auto kArraySize = 4;
    // NOLINTBEGIN
    raii::unique_ptr<int[], raii::deleter_wrapper<std::default_delete<int[]>>> arrayWithStdDeleter{
      new int[kArraySize]
    };
    arrayWithStdDeleter[0] = -1;
    arrayWithStdDeleter[1] = 4;
    arrayWithStdDeleter[2] = 7;

    arrayWithStdDeleter.reset(new int[2]);
    // NOLINTEND
  }
  return 0;
}
