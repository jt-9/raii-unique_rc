#include <catch2/catch_test_macros.hpp>

#include "urc/unique_ptr.hpp"

#include <utility>

namespace {
struct CopyAssignDeleter
{
  CopyAssignDeleter() = default;
  ~CopyAssignDeleter() = default;

  CopyAssignDeleter(const CopyAssignDeleter &) = default;
  CopyAssignDeleter(CopyAssignDeleter &&) = default;

  // NOLINTNEXTLINE(cert-oop54-cpp) false poisitive
  CopyAssignDeleter &operator=(const CopyAssignDeleter & /*unused*/) noexcept
  {
    CHECK(true);
    return *this;
  }

  CopyAssignDeleter &operator=(CopyAssignDeleter && /*unused*/) noexcept
  {
    CHECK(false);
    return *this;
  }

  template<class T> void operator()(T * /*unused*/) const noexcept {}
};

struct DerivedCopyAssignDeleter : CopyAssignDeleter
{
};
}// namespace


TEST_CASE("unique_ptr move assignment forwards deleter", "[unique_ptr][unique_ptr::operator=]")
{
  CopyAssignDeleter baseDeleter;

  // NOLINTNEXTLINE(readability-isolate-declaration)
  raii::unique_ptr<int, CopyAssignDeleter &> ptr1{ nullptr, baseDeleter }, ptr2{ nullptr, baseDeleter };
  ptr2 = std::move(ptr1);

  DerivedCopyAssignDeleter derivedDeleter;

  // NOLINTNEXTLINE
  raii::unique_ptr<int[], CopyAssignDeleter &> aptr1{ nullptr, derivedDeleter }, aptr2{ nullptr, derivedDeleter };
  aptr2 = std::move(aptr1);
}