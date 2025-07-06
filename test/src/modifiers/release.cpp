#include <catch2/catch_test_macros.hpp>

#include "urc/memory_delete.hpp"
#include "urc/unique_rc.hpp"
// #include "urc/unique_ptr.hpp"


TEST_CASE("Release value initialised unique_rc<float*, memory_delete<float*>>", "[unique_rc][release]")
{
  constexpr auto test_number = 496.0F;
  raii::unique_rc<float *, raii::memory_delete<float *>> rc1{ new float{ test_number } };

  CHECK(rc1);
  CHECK(rc1.get() != nullptr);

  decltype(rc1) init_from_release{ rc1.release() };
  CHECK(init_from_release);

  CHECK(rc1.get() == nullptr);
  CHECK_FALSE(rc1);
}

TEST_CASE("Release default unique_rc<T *, memory_delete<T *>>", "[unique_rc][release]")
{
  raii::unique_rc<int *, raii::memory_delete<int *>> ptr1;
  CHECK_FALSE(ptr1);
  CHECK(ptr1.get() == nullptr);

  int const *raw_ptr = ptr1.release();
  CHECK(raw_ptr == nullptr);
  CHECK_FALSE(ptr1);
}