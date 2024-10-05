# unique_rc


raii::unique_rc is a small open-source C++ library which provides [RAII](https://https://en.cppreference.com/w/cpp/language/raii) wrapper for generic resource - anything that needs to be acquired, used, and eventually released.

If you like this project, please consider donating to one of the funds
that help victims of the war in Ukraine: <https://www.stopputin.net/>.

## About:


The idea behind **raii::unique_rc** is similar to [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr) in providing automatic and exception-safe deallocation of dynamically allocated resource be it a memory, file descriptors, file handles, events, sockets, COM objects, GDI objects, etc.

The key difference is a requirement for 'deallocator', a [functional object](https://en.cppreference.com/w/cpp/named_req/FunctionObject) which deallocates resource, to provide what is viewed as invalid value. This approach is described in Kenny Kerr's artcle [Windows with C++ - C++ and the Windows API](https://learn.microsoft.com/en-us/archive/msdn-magazine/2011/july/msdn-magazine-windows-with-c-c-and-the-windows-api). **raii::unique_rc** provides *similar interface* to std::unique_ptr, but unlike latter, which compares against `nullptr`, **raii::unique_rc** allows deallocator to specify a different invalid value e.g. `INVALID_HANDLE_VALUE, INVALID_SOCKET, etc`.

## Documentation:


To be added

## Features:

- Interface similar to std::unique_ptr, with extra `invalid_value_type` defined in deallocator.
- Small code size consisting of just several files `unique_rc.hpp`, `defs.hpp` and deallocators.
- Clean warning-free codebase even on high warning levels such as `-Wall` `-Wextra` `-pedantic`.

## Examples:

#### 1. File descriptor

``` c++
#include "unique_rc.hpp"
#include "stdio_fclose.hpp"

#include <stdio.h>

int main() {
  raii::unique_rc<FILE *, raii::stdio_fclose> streamRC{ std::fopen("/tmp/test.txt", "r+") };
  if (streamRC) {
    // Do work here...
    // Use streamRC.get() to get file descriptor
  }
}
```

  with stdio [fclose](https://en.cppreference.com/w/c/io/fclose) deallocator
``` c++
struct stdio_fclose
{
  constexpr stdio_fclose() noexcept = default;

  [[nodiscard]] static constexpr std::nullptr_t invalid() noexcept { return nullptr; }

  void operator()(FILE *h) const noexcept { fclose(h); }
};
```

#### 2. CreateFile

``` c++
#include "unique_rc.hpp"
#include "windows_close_handle.hpp"

#include <windows.h>

int main() {
  raii::unique_rc<HANDLE, 
      raii::close_handle_invalid_handle_value<HANDLE>> myFile{ CreateFile(/* params */) };
  if (myFile) {
    // Do your work...
    // Use myFile.get() to get file HANDLE
  }
}
```

  with [CloseHandle](https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle) deallocator
``` c++
template<typename Handle = HANDLE>
  requires std::convertible_to<Handle, HANDLE>
struct close_handle_invalid_handle_value
{
  constexpr close_handle_invalid_handle_value() noexcept = default;

  [[nodiscard]] static constexpr Handle invalid() noexcept { return INVALID_HANDLE_VALUE; }

  void operator()(Handle h) const noexcept { CloseHandle(h); }
};
```
## Acknowledgments
- [Kenny Kerr](https://github.com/kennykerr) and his article [Windows with C++ - C++ and the Windows API](https://learn.microsoft.com/en-us/archive/msdn-magazine/2011/july/msdn-magazine-windows-with-c-c-and-the-windows-api)
- [Jason Turner](https://github.com/lefticus) and his awesome tutorial on cmake quick start [C++ Weekly - Ep 208 - The Ultimate CMake / C++ Quick Start](https://youtu.be/YbgH7yat-Jo?si=YK9MUqr6yeluE5c4) and [C++ Weekly - Ep 376 - Ultimate CMake C++ Starter Template (2023 Updates)](https://youtu.be/ucl0cw9X3e8?si=Ma6CT5jBd5qlL3Ft)

