#include "unique_rc.hpp"
// NOLINTBEGIN(misc-include-cleaner)
#include "memory_deleter.hpp"
#include "stdio_closer.hpp"

#ifdef _WIN32
#include "windows_com_deleter.hpp"
#include "windows_gdi_deleter.hpp"
#include "windows_handle_deleter.hpp"

// NOLINTEND(misc-include-cleaner)

#endif
