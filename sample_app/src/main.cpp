// This file will be generated automatically when cur_you run the CMake
// configuration step. It creates a namespace called `myproject`. You can modify
// the source template at `configured_files/config.hpp.in`.
#include "internal_use_only/config.hpp"

#include "memory_deleter.hpp"
#include "test_policies.hpp"
#include "unique_rc.hpp"

#include <memory>

#include <string>
#include <string_view>

// #include <print>
// #include <iostream>
#include <fmt/core.h>

namespace playground {
void measureAndPrintUniquePtrSize() noexcept
{
  using namespace std::literals;

  std::unique_ptr<int, raii::memory_delete<int *>> ptr1{ new int(1001) };

  //		std::default_delete<int> p;

  //		std::cout << "Sizeof 'std::unique_ptr<int, raii::memory_delete<int*>>' is "sv << sizeof(ptr1) << "
  //bytes\n"sv; 		std::println("Sizeof 'std::unique_ptr<int, raii::memory_delete<int*>>' is {} bytes"sv, sizeof(ptr1));
  fmt::println("Sizeof 'std::unique_ptr<int, raii::memory_delete<int*>>' is {} bytes"sv, sizeof(ptr1));

  // std::_Compressed_pair<raii::memory_delete<int*>, int*> cp{ std::_Zero_then_variadic_args_t{} };
  // std::cout << "Sizeof 'std::_Compressed_pair<raii::memory_delete<int*>, int*>' is "sv << sizeof(cp) << " bytes\n"sv;

  std::tuple<int *, raii::memory_delete<int *>> tp{};
  //		std::cout << "Sizeof 'std::tuple<int*, raii::memory_delete<int*>>' is "sv << sizeof(tp) << " bytes\n"sv;
  //		std::println("Sizeof 'std::tuple<int*, raii::memory_delete<int*>>' is {} bytes"sv, sizeof(tp));
  fmt::println("Sizeof 'std::tuple<int*, raii::memory_delete<int*>>' is {} bytes"sv, sizeof(tp));
}
}// namespace playground


int main([[maybe_unused]] int argc, const char *argv[]) noexcept
{
  using namespace std::literals;

  fmt::println("{0} Version {1}.{2}.{3}", argv[0], raii::cmake::project_version_major, raii::cmake::project_version_minor, raii::cmake::project_version_patch);

  playground::measureAndPrintUniquePtrSize();

  using namespace raii;

  {
    unique_rc<int *, PointerWithDummy2<int *, int, char>> rc1{ new int{ 4 },
      PointerWithDummy2<int *, int, char>{ -4, 'A' } };
    unique_rc<int *, PointerWithDummy2<int *, int, char>> rc2{ new int{ 1 },
      PointerWithDummy2<int *, int, char>{ 11, 'u' } };

    rc1.swap(rc2);

    rc1 = std::move(rc2);
  }

  {
    std::unique_ptr<int, PointerWithDummy2<int *, int, char>> rc1{ new int{ 4 },
      PointerWithDummy2<int *, int, char>{ -4, 'A' } };
  }

  {
    unique_rc<int *, memory_delete<int *>> rc1{ new int{ 64 } }, rc2{ new int{ 1331 } };

    rc1.swap(rc2);

    rc1 = std::move(rc2);

    rc1.reset();
    rc1.reset(rc1.get());

    // std::cout << "Sizeof unique_rc<int*, memory_delete<int*> of long long is "sv << sizeof(rc1) << " bytes\n"sv;
    fmt::println("Sizeof unique_rc<int*, memory_delete<int*> of long long is {} bytes"sv, sizeof(rc1));
  }

  return 0;
}
