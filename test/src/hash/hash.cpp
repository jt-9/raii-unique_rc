#include <catch2/catch_test_macros.hpp>

// #include "memory_delete.hpp"
#include "unique_ptr.hpp"
// #include "unique_rc.hpp"

//#include <functional>

TEST_CASE("Test hash for unique_ptr{new T{}}, and unique_ptr{new T[]]}", "[unique_ptr][hash]")
{
  struct T
  {
  };

  const raii::unique_ptr<T> ptr0{ new T{} };
//   const std::hash<raii::unique_ptr<T>> hash_unique_ptr0;
//   const std::hash<typename raii::unique_ptr<T>::pointer> hash_ptr0;

//   REQUIRE(hash_unique_ptr0(ptr0) == hash_ptr0(ptr0.get()));

//   const raii::unique_ptr<T[]> ptr1{ new T[10] };
//   std::hash<raii::unique_ptr<T[]>> hu1;
//   std::hash<typename raii::unique_ptr<T[]>::pointer> hp1;

//   REQUIRE(hu1(ptr1) == hp1(ptr1.get()));
}
