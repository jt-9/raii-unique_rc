#include "unique_rc.hpp"

// NOLINTBEGIN(misc-include-cleaner)
#include "coroutine_destroy.hpp"
#include "memory_delete.hpp"
#include "stdio_fclose.hpp"

#ifdef _WIN32
#include "windows_close_handle.hpp"
#include "windows_com_file_dlg_unadvise.hpp"
#include "windows_com_release.hpp"
#include "windows_dc_delete.hpp"
#include "windows_dc_release.hpp"
#include "windows_dc_restore.hpp"
#include "windows_delete_gdi_object.hpp"
#include "windows_select_gdi_object.hpp"
#endif
// NOLINTEND(misc-include-cleaner)
