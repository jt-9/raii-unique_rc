#ifndef CONSTEVAL_LAMBDA_EXAMPLE_HPP
#define CONSTEVAL_LAMBDA_EXAMPLE_HPP

#pragma once

#include "urc/unique_ptr.hpp"

#include <cassert>
// #include <memory>
#include <concepts>
#include <utility>//move

namespace {
using IntUPtr = raii::unique_ptr<int>;
}

namespace raii_sample {

void testTypeConstructAssignWithConsteval() noexcept
{
  // Requires c++23, doesn't work in MSVC 2022
  /*/
  [] consteval {
    IntUPtr empty;
    assert(!empty);
  }();

  [](IntUPtr::element_type val) consteval {
    IntUPtr filled{ new int{ val } };
    assert(val == *filled);
  }(77);

  [](IntUPtr::element_type val) consteval {
    IntUPtr src{ new int{ val } };
    assert(val == *src);

    const auto dst = std::move(src);
    assert(val == *dst);
  }(42);

  [](IntUPtr::element_type val) consteval {
    IntUPtr src{ new int{ val } };
    assert(val == *src);

    IntUPtr dst;
    dst = std::move(src);
    assert(val == *dst);
  }(314);

  [](IntUPtr::element_type val1, IntUPtr::element_type val2) consteval {
    IntUPtr src{ new int{ val1 } };
    assert(val1 == *src);

    IntUPtr dst{ new int{ val2 } };
    assert(val2 == *dst);

    std::ranges::swap(src, dst);

    assert(val2 == *src);
    assert(val1 == *dst);
  }(42, -4);
  //*/
}

}// namespace raii_sample

#endif// CONSTEVAL_LAMBDA_EXAMPLE_HPP