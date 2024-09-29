// SmartHandle.h : Defines template class for handles
//

#ifndef TEST_POLICY_HPP
#define TEST_POLICY_HPP

#include "defs.hpp"

#include <concepts>
#include <utility>

#include <new>
#include <type_traits>

RAII_NS_BEGIN

template<typename Handle>
  requires std::is_pointer_v<Handle>
class PointerWithDummy0
{
public:
  using handle = Handle;

  inline constexpr void operator()(Handle h) const noexcept { delete h; }

  [[nodiscard]] inline static constexpr Handle invalid() noexcept { return nullptr; }
};

template<typename Handle, typename Dummy1>
  requires std::is_pointer_v<Handle>
class PointerWithDummy1
{
public:
  using handle = Handle;

  raii_inline constexpr explicit PointerWithDummy1(Dummy1 nd1) noexcept : d1{ nd1 } {}

  raii_inline constexpr void operator()(Handle h) const noexcept { delete h; }

  [[nodiscard]] raii_inline static constexpr Handle invalid() noexcept { return nullptr; }

protected:
  Dummy1 d1;
};


template<typename Handle, typename Dummy1, typename Dummy2>
  requires std::is_pointer_v<Handle>
class PointerWithDummy2
{
public:
  using handle = Handle;

  raii_inline constexpr PointerWithDummy2(Dummy1 nd1, Dummy2 nd2) noexcept : d1{ nd1 }, d2{ nd2 } {}

  raii_inline constexpr void operator()(Handle h) const noexcept { delete h; }

  [[nodiscard]] raii_inline static constexpr Handle invalid() noexcept { return nullptr; }

protected:
  Dummy1 d1;
  Dummy2 d2;
};

RAII_NS_END

#endif// WinHandlePolicies_h
