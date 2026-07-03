#include <catch2/catch_test_macros.hpp>


// #include "urc/unique_rc.hpp"
#include "urc/memory_free.hpp"
#include "urc/unique_ptr.hpp"

#include <cstdlib>// std::free
#include <type_traits>// std::is_pointer_v


namespace {

template<typename Handle>
  requires std::is_pointer_v<Handle>
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
struct DestructionTracker : public raii::memory_free<Handle>
{
  using Base = raii::memory_free<Handle>;

  constexpr DestructionTracker() noexcept : free_called{ 0 } {}
  constexpr ~DestructionTracker() noexcept = default;

  template<typename U>
    requires std::is_convertible_v<U, Handle>
  explicit constexpr DestructionTracker(const DestructionTracker<U> &src) noexcept : Base(src), free_called{ 0 }
  {}

  // cppcheck-suppress duplInheritedMember;
  constexpr void operator()(Handle ptr) noexcept
  {
    Base::operator()(ptr);
    ++free_called;
  }

  int free_called;
};
}// namespace


TEST_CASE("deleter operator() should be called once", "[unique_ptr][operator()][deleter]")
{
  constexpr std::size_t kArraySize = 10;
  using TypeOfUrcMalloc = int;
  raii::unique_ptr<TypeOfUrcMalloc, DestructionTracker<TypeOfUrcMalloc *>> ptr1{
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, hicpp-no-malloc)
    static_cast<TypeOfUrcMalloc *>(std::malloc(kArraySize * sizeof(TypeOfUrcMalloc))),
    DestructionTracker<TypeOfUrcMalloc *>{}
  };

  CHECK(0 == ptr1.get_deleter().free_called);

  ptr1.reset();

  CHECK(1 == ptr1.get_deleter().free_called);
}