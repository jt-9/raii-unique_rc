#ifndef LTU_STD_PRINT_STRATEGY
#define LTU_STD_PRINT_STRATEGY

#include <version>

#ifdef __cpp_lib_print

#include <print>
#include <utility>


namespace ltu {

struct StdPrintStrategy
{
  template<class... Types>
  static constexpr void println(const std::format_string<Types...> format, Types &&...args) noexcept
  {
    std::println(format, std::forward<Types>(args)...);
  }

  static void println() noexcept { std::println(); }

  template<class... Types>
  static constexpr void print(const std::format_string<Types...> format, Types &&...args) noexcept
  {
    std::print(format, std::forward<Types>(args)...);
  }

protected:
  ~StdPrintStrategy() = default;
};// StdPrintStrategy

}// namespace ltu

#endif// __cpp_lib_print

#endif// LTU_STD_PRINT_STRATEGY
