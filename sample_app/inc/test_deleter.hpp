// SmartHandle.h : Defines template class for handles
//

#ifndef TEST_DELETER_HPP
#define TEST_DELETER_HPP

#include <concepts>
#include <utility>

#include <new>
#include <type_traits>

template<typename Handle>
  requires std::is_pointer_v<Handle>
class PointerWithDummy0
{
public:
  using handle = Handle;

  constexpr void operator()(Handle h) const noexcept { delete h; }

  [[nodiscard]] static constexpr auto invalid() noexcept { return nullptr; }
};

template<typename Handle, typename Dummy1>
  requires std::is_pointer_v<Handle>
class PointerWithDummy1
{
public:
  using handle = Handle;

  constexpr explicit PointerWithDummy1(Dummy1 nd1) noexcept : d1{ nd1 } {}

  constexpr void operator()(Handle h) const noexcept { delete h; }

  [[nodiscard]] static constexpr auto invalid() noexcept { return nullptr; }
  [[nodiscard]] static constexpr bool is_valid(Handle h) noexcept { return h != invalid(); }

protected:
  Dummy1 d1;
};


template<typename Handle, typename Dummy1, typename Dummy2>
  requires std::is_pointer_v<Handle>
class PointerWithDummy2
{
public:
  using handle = Handle;

  constexpr PointerWithDummy2(Dummy1 nd1, Dummy2 nd2) noexcept : d1{ nd1 }, d2{ nd2 } {}

  constexpr void operator()(Handle h) const noexcept { delete h; }

  [[nodiscard]] static constexpr auto invalid() noexcept { return nullptr; }
  [[nodiscard]] static constexpr bool is_valid(Handle h) noexcept { return h; }

protected:
  Dummy1 d1;
  Dummy2 d2;
};

template<typename Handle>
  requires std::is_pointer_v<Handle>
class NoStaticInvalidPointerDummy0
{
public:
  using handle = Handle;

  constexpr void operator()(Handle h) const noexcept { delete h; }

  //[[nodiscard]] constexpr auto invalid() noexcept { return nullptr; }
};


#endif// TEST_DELETER_HPP
