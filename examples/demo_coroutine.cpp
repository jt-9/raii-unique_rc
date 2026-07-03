
#include "urc/unique_coroutine_handle.hpp"

#include "CallLog.hpp"
#include "StdPrint.hpp"

#include <concepts>
#include <coroutine>
#include <cstdio>// std::puts
#include <exception>// std::terminate
#include <iterator>// std::default_sentinel_t
#include <optional>
#include <print>
#include <utility>// std::move


namespace raii_sample {

// Example from https://en.cppreference.com/cpp/coroutine/coroutine_handle
//  uses raii::unique_coroutine_handle<> to manage std::coroutine_handle
template<typename /*std::movable*/ T>
  requires std::movable<T>
class Generator
{
public:
  struct promise_type
  {
    Generator<T> get_return_object()
    {
      const ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy);
      return Generator{ CoroHandle::from_promise(*this) };
    }
    static std::suspend_always initial_suspend() noexcept
    {
      const ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy);
      return {};
    }
    static std::suspend_always final_suspend() noexcept
    {
      const ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy);
      return {};
    }
    std::suspend_always yield_value(T value) noexcept
    {
      // ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy);
      current_value = std::move(value);
      return {};
    }
    // Disallow co_await in generator coroutines.
    void await_transform() = delete;

    [[noreturn]] static void unhandled_exception()
    {
      const ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy);
      std::terminate();
    }

    static void return_void() noexcept { const ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy); }

    std::optional<T> current_value;
  };// promise_type


  using CoroutineHolder = raii::unique_coroutine_handle<promise_type>;
  using CoroHandle = CoroutineHolder::handle;

  explicit Generator(const CoroHandle coroutine) noexcept : m_coroutine{ coroutine }
  { const ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy); }

  constexpr Generator() noexcept { const ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy); }

  constexpr ~Generator() noexcept { const ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy); }

  Generator(const Generator &) = delete;
  Generator &operator=(const Generator &) = delete;

  constexpr Generator(Generator &&other) noexcept : m_coroutine{ std::move(other.m_coroutine) }
  { ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy); }

  constexpr Generator &operator=(Generator &&other) noexcept
  {
    ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy);

    m_coroutine = std::move(other.m_coroutine);
    return *this;
  }

  // Range-based for loop support.
  class Iter
  {
  public:
    Iter &operator++() noexcept
    {
      m_coroutine.resume();
      return *this;
    }

    const T &operator*() const
    {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      return *m_coroutine.promise().current_value;
    }

    bool operator==(std::default_sentinel_t /*unused*/) const { return !m_coroutine || m_coroutine.done(); }

    explicit Iter(const CoroHandle &coroutine) noexcept : m_coroutine{ coroutine } {}

  private:
    CoroHandle m_coroutine;
  };

  constexpr Iter begin() const noexcept
  {
    if (m_coroutine) { m_coroutine.get().resume(); }

    return Iter{ m_coroutine.get() };
  }

  static constexpr std::default_sentinel_t end() noexcept { return {}; }

private:
  CoroutineHolder m_coroutine;
};

template<std::integral T> Generator<T> range(T first, const T last)
{
  const ltu::LOG_FUNC_INOUT(ltu::StdPrintStrategy);
  while (first < last) { co_yield first++; }
}

}// namespace raii_sample


// NOLINTNEXTLINE(bugprone-exception-escape)
int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) noexcept
{
  std::puts("=======================================================");
  std::println("Demonstrating simple coroutine generator...");
  constexpr auto lastElem = 123;
  for (const auto rangeElem : raii_sample::range(65, lastElem)) {
    std::print("{:c} ", rangeElem);
    if (rangeElem == lastElem - 1) { std::println(); }
  }
  // std::cout << '\n';

  return 0;
}
