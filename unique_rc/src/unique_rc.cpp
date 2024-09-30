#include "unique_rc.hpp"
//#include "memory_deleter.hpp"

#ifdef _WIN32
#include "windows_com_deleter.hpp"
#include "windows_gdi_deleter.hpp"
#include "windows_handle_deleter.hpp"
#endif
