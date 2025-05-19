#include <catch2/catch_test_macros.hpp>

//#include "memory_delete.hpp"
#include "unique_ptr.hpp"
#include "unique_rc.hpp"

#include <cstddef>//std::nullptr_t
#include <type_traits>

TEST_CASE("DR 2899 is_(nothrow_)move_constructible unique_rc", "[unique_rc::unique_rc]")
{
  // NOLINTBEGIN(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
  struct Del
  {
    static constexpr std::nullptr_t invalid() noexcept;
    static constexpr bool is_owned(int *) noexcept;

    Del() = default;
    ~Del() = default;

    Del(Del &&) = delete;

    void operator()(int *) const;
  };
  // NOLINTEND(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)

  STATIC_CHECK_FALSE(std::is_move_constructible_v<raii::unique_rc<int *, Del>>);
  STATIC_CHECK(std::is_move_constructible_v<raii::unique_rc<int *, Del &>>);
}

TEST_CASE("DR 2899 is_(nothrow_)move_assignable unique_rc", "[unique_rc::operator=]")
{
  // NOLINTBEGIN(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
  struct Del2
  {
    static constexpr std::nullptr_t invalid() noexcept;
    static constexpr bool is_owned(int *) noexcept;

    Del2() = default;
    ~Del2() = default;

    Del2(Del2 &&) = default;
    Del2 &operator=(Del2 &&) = delete;
    Del2 &operator=(const Del2 &) = default;

    void operator()(int *) const;
  };
  // NOLINTEND(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)

  STATIC_CHECK_FALSE(std::is_move_assignable_v<raii::unique_rc<int *, Del2>>);
  STATIC_CHECK_FALSE(std::is_move_assignable_v<raii::unique_ptr<int, Del2>>);

  STATIC_CHECK(std::is_move_assignable_v<raii::unique_rc<int *, Del2 &>>);
  STATIC_CHECK(std::is_move_assignable_v<raii::unique_ptr<int, Del2 &>>);
  // STATIC_CHECK_FALSE(std::is_move_assignable_v<raii::deleter_class_wrapper<Del2>>);
  // STATIC_CHECK_FALSE(std::is_move_assignable_v<raii::unique_ptr<int, raii::deleter_class_wrapper<Del2>>>);
  // STATIC_CHECK(std::is_move_assignable_v<raii::unique_ptr<int, raii::deleter_class_wrapper<Del2> &>>);

  // NOLINTBEGIN(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
  struct Del3
  {
    static constexpr std::nullptr_t invalid() noexcept;
    static constexpr bool is_owned(int *) noexcept;

    Del3() = default;
    ~Del3() = default;

    Del3(Del3 &&) = default;
    Del3 &operator=(Del3 &&) = default;
    Del3 &operator=(const Del3 &) = delete;

    void operator()(int *) const;
  };
  // NOLINTEND(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)

  STATIC_CHECK(std::is_move_assignable_v<raii::unique_rc<int *, Del3>>);
  STATIC_CHECK(std::is_move_assignable_v<raii::unique_ptr<int, Del3>>);

  STATIC_CHECK_FALSE(std::is_move_assignable_v<raii::unique_rc<int *, Del3 &>>);
  STATIC_CHECK_FALSE(std::is_move_assignable_v<raii::unique_ptr<int, Del3 &>>);
}