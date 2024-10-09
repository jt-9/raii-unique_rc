include(GenerateExportHeader)
#include(CMakePrintHelpers)

# function(add_headers VAR)
#   set(headers ${${VAR}})
#   foreach (header ${ARGN})
#     set(headers ${headers} inc/${header})
#   endforeach()
#   set(${VAR} ${headers} PARENT_SCOPE)
# endfunction()

set(HEADERS 
  ${CMAKE_CURRENT_LIST_DIR}/inc/defs.hpp
  ${CMAKE_CURRENT_LIST_DIR}/inc/unique_rc.hpp
  ${CMAKE_CURRENT_LIST_DIR}/inc/deleter/memory_delete.hpp
  ${CMAKE_CURRENT_LIST_DIR}/inc/deleter/stdio_fclose.hpp
  ${CMAKE_CURRENT_LIST_DIR}/inc/deleter/windows_com_release.hpp
  ${CMAKE_CURRENT_LIST_DIR}/inc/deleter/windows_com_file_dlg_unadvise.hpp
  ${CMAKE_CURRENT_LIST_DIR}/inc/deleter/windows_gdi_deleter.hpp
  ${CMAKE_CURRENT_LIST_DIR}/inc/deleter/windows_dc_deleter.hpp
  ${CMAKE_CURRENT_LIST_DIR}/inc/deleter/windows_close_handle.hpp
)

set(SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/src/unique_rc.cpp
)

function(add_mylib_statics lib_name)
  add_library(${lib_name} STATIC ${HEADERS} ${SOURCES})
  #target_compile_features(${lib_name} PUBLIC cxx_std_20)
  target_link_libraries(${lib_name} PRIVATE 
                myproject::myproject_options 
                myproject::myproject_warnings)

  #cmake_print_variables(PROJECT_SOURCE_DIR)
  #cmake_print_variables(CMAKE_CURRENT_LIST_DIR)

  target_include_directories(${lib_name} ${WARNING_GUARD} PUBLIC 
                                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
                                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/deleter>
                                $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

set_target_properties(${lib_name} PROPERTIES
                LINKER_LANGUAGE CXX 
                VERSION ${PROJECT_VERSION}
                DEBUG_POSTFIX "d")
endfunction()


add_mylib_statics(urc)
add_library(myproject::urc ALIAS urc)


#generate_export_header(unique_rc EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/unique_rc/include/unique_rc_export.hpp)
