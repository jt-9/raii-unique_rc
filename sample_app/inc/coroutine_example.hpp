#ifndef COROUTINE_EXAMPLE_HPP
#define COROUTINE_EXAMPLE_HPP

#pragma once

#include "urc/coroutine_destroy.hpp"
#include "urc/unique_rc.hpp"

#include <coroutine>
#include <optional>

namespace raii_sample {
template<typename /*std::movable*/ T>
  requires std::movable<T>
class Generator
{
public:
  struct promise_type
  {
    Generator<T> get_return_object() { return Generator{ CoroHandle::from_promise(*this) }; }
    static std::suspend_always initial_suspend() noexcept { return {}; }
    static std::suspend_always final_suspend() noexcept { return {}; }
    std::suspend_always yield_value(T value) noexcept
    {
      current_value = std::move(value);
      return {};
    }
    // Disallow co_await in generator coroutines.
    void await_transform() = delete;

    [[noreturn]] static void unhandled_exception() { std::terminate(); }

    static void return_void() noexcept {}

    std::optional<T> current_value;
  };// promise_type

  using CoroHandle = std::coroutine_handle<promise_type>;
  using CoroutineHolder = raii::unique_rc<CoroHandle, raii::coroutine_destroy, raii::resolve_handle_type, CoroHandle, raii::coro_invalid_handle_policy>;

  explicit Generator(const CoroHandle coroutine) noexcept : m_coroutine{ coroutine } {}

  Generator() = default;
  ~Generator() = default;

  Generator(const Generator &) = delete;
  Generator &operator=(const Generator &) = delete;

  // Generator(Generator &&other) noexcept : m_coroutine{ other.m_coroutine } { other.m_coroutine = {}; }
  // Generator &operator=(Generator &&other) noexcept
  //{
  //   if (this != &other) {
  //     if (m_coroutine) m_coroutine.destroy();
  //     m_coroutine = other.m_coroutine;
  //     other.m_coroutine = {};
  //   }
  //   return *this;
  // }

  // Range-based for loop support.
  class Iter
  {
  public:
    Iter &operator++() noexcept
    {
      m_coroutine.resume();
      return *this;
    }
    const T &operator*() const { return *m_coroutine.promise().current_value; }
    bool operator==(std::default_sentinel_t) const { return !m_coroutine || m_coroutine.done(); }

    // cppcheck-suppress passedByValueCallback pass a copy to coroutine
    explicit Iter(const CoroHandle coroutine) noexcept : m_coroutine{ coroutine } {}

  private:
    CoroHandle m_coroutine;
  };

  constexpr Iter begin() const noexcept
  {
    if (m_coroutine) m_coroutine.get().resume();
    return Iter{ m_coroutine.get() };
  }

  static constexpr std::default_sentinel_t end() noexcept { return {}; }

private:
  CoroutineHolder m_coroutine;
};

template<std::integral T> Generator<T> range(T first, const T last)
{
  while (first < last) co_yield first++;
}

}// namespace raii_sample


#endif// COROUTINE_EXAMPLE_HPP
