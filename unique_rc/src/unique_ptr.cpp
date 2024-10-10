#include "unique_ptr.hpp"

// NOLINTBEGIN(misc-include-cleaner)
#include "memory_delete.hpp"
#include "stdio_fclose.hpp"

#ifdef _WIN32
#include "windows_close_handle.hpp"
#include "windows_com_file_dlg_unadvise.hpp"
#include "windows_com_release.hpp"
#include "windows_dc_deleter.hpp"
#include "windows_gdi_deleter.hpp"
// NOLINTEND(misc-include-cleaner)

#endif