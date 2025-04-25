#include <catch2/catch_test_macros.hpp>


#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

namespace {
constexpr bool test_reset() noexcept
{
  {
    raii::unique_ptr<int> ptr1;
    ptr1.reset();
    // STATIC_REQUIRE(!ptr1);

    ptr1.reset(nullptr);
    // STATIC_REQUIRE(!ptr1);
    // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, cppcoreguidelines-owning-memory)
    ptr1.reset(new int(2));
    // STATIC_REQUIRE(*ptr1 == 2);

    // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, cppcoreguidelines-owning-memory)
    ptr1.reset(new int(3));
    // STATIC_REQUIRE(*ptr1 == 3);
    ptr1.reset(nullptr);
    // STATIC_REQUIRE(!ptr1);
  }

  {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
    raii::unique_ptr<int[]> ptra1;
    ptra1.reset();
    // STATIC_REQUIRE(!ptra1);
    ptra1.reset(nullptr);
    // STATIC_REQUIRE(!ptra1);

    // cppcheck-suppress leakNoVarFunctionCall false positive
    // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, cppcoreguidelines-owning-memory)
    ptra1.reset(new int[]{ 2, 3 });
    //STATIC_REQUIRE(ptra1[0] == 2);

    // NOLINTNEXTLINE(readability-isolate-declaration)
    const auto c_Elem0 = 4, c_Elem1 = 5, c_Elem2 = 6;

    // cppcheck-suppress leakNoVarFunctionCall false positive
    // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, cppcoreguidelines-owning-memory)
    ptra1.reset(new int[]{ c_Elem0, c_Elem1, c_Elem2 });
    // STATIC_REQUIRE(ptra1[1] == c_Elem1);
    ptra1.reset(nullptr);
    // STATIC_REQUIRE(!ptra1);
  }

  {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
    raii::unique_ptr<const int[]> ptra2;

    // cppcheck-suppress leakNoVarFunctionCall false positive
    // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new, cppcoreguidelines-owning-memory)
    ptra2.reset(new int[2]{});
  }
  return true;
}
}// namespace

TEST_CASE("constexpr unique_ptr::reset", "[unique_ptr][reset]") { STATIC_REQUIRE(test_reset()); }

// constexpr bool test_release()
// {
//   std::unique_ptr<int> ptr1;
//   int *raw_ptr = ptr1.release();
//   VERIFY(!raw_ptr);
//   VERIFY(!ptr1);

//   std::unique_ptr<int> p2(new int(2));
//   raw_ptr = p2.release();
//   VERIFY(raw_ptr);
//   VERIFY(!p2);
//   delete raw_ptr;

//   std::unique_ptr<int[]> a1;
//   raw_ptr = a1.release();
//   VERIFY(!raw_ptr);
//   VERIFY(!a1);

//   std::unique_ptr<int[]> a2(new int[2]{});
//   raw_ptr = a2.release();
//   VERIFY(raw_ptr);
//   VERIFY(!a2);
//   delete[] raw_ptr;

//   return true;
// }
// static_assert(test_release());