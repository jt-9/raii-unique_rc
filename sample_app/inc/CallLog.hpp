#ifndef LTU_CALL_LOGGER
#define LTU_CALL_LOGGER

#include <cstdint>
#include <source_location>
#include <string>
#include <string_view>
#include <thread>

namespace ltu {

// Logs in and out of a scope
template<class PrintStrategy> struct ScopedLogger
{
  static thread_local std::size_t indent;
  std::string_view name_;

  explicit constexpr ScopedLogger(std::string_view name, std::uint_least32_t line) noexcept : name_{ name }
  {
    PrintStrategy::println(
      "{}\\{}\n\tat line: {}\n\tthread id: {}", std::string(indent, ' '), name_, line, std::this_thread::get_id());
    indent++;
  }

  constexpr ~ScopedLogger() noexcept
  {
    indent--;
    PrintStrategy::println("{}/{}\n\tthread id: {}", std::string(indent, ' '), name_, std::this_thread::get_id());
  }

  // ScopedLogger is non-movable, non-copyable
  ScopedLogger(ScopedLogger &&) = delete;
  auto operator=(ScopedLogger &&) = delete;
};

template<class PrintStrategy>
thread_local std::size_t ScopedLogger<PrintStrategy>::indent = 0;// Should go in cpp file, if we have one

// Creates a ScopedLogger for the current function
#define LOG_FUNC_INOUT(PrintStrat)          \
  ScopedLogger<PrintStrat> logger##__LINE__ \
  { std::source_location::current().function_name(), std::source_location::current().line() }

}// namespace ltu

#endif// LTU_CALL_LOGGER