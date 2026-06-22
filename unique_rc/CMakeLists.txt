include(GenerateExportHeader)
#include(CMakePrintHelpers)


function(add_lib_interface lib_name)
  add_library(${lib_name} INTERFACE)
  
  target_link_libraries(${lib_name} INTERFACE 
                urc::project_options 
                urc::project_warnings)

  #cmake_print_variables(PROJECT_SOURCE_DIR)
  #cmake_print_variables(CMAKE_CURRENT_LIST_DIR)

  target_sources(${lib_name} ${WARNING_GUARD}
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ./include
    FILES include/urc/raii_defs.hpp
          include/urc/concepts.hpp
          include/urc/coroutine_destroy.hpp
          include/urc/memory_delete.hpp
          include/urc/stdio_fclose.hpp

          include/urc/unique_rc.hpp
          include/urc/unique_ptr.hpp
          include/urc/unique_coroutine_handle.hpp
  )

  if (WIN32)
    target_sources(${lib_name} ${WARNING_GUARD}
      INTERFACE
      FILE_SET HEADERS
      BASE_DIRS ./include
      FILES include/urc/deleter_windows_gdi.hpp
          include/urc/deleter_windows_kernel.hpp
    )
  endif()

  if (WIN32 AND RAII_URC_ENABLE_COM_DELETERS)
    target_sources(${lib_name} ${WARNING_GUARD}
      INTERFACE
      FILE_SET HEADERS
      BASE_DIRS ./include
      FILES include/urc/deleter_com.hpp
          include/urc/deleter_com_shell.hpp
    )
  endif()
endfunction()


add_lib_interface(urc)
add_library(urc::urc ALIAS urc)

#urc library uses c++ 20 which is set via urc::project_options


#generate_export_header(unique_rc EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/unique_rc/include/unique_rc_export.hpp)
