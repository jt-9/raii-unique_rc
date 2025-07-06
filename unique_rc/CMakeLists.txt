include(GenerateExportHeader)
#include(CMakePrintHelpers)


function(add_lib_interface lib_name)
  add_library(${lib_name} INTERFACE)
  
  target_link_libraries(${lib_name} INTERFACE 
                myproject::myproject_options 
                myproject::myproject_warnings)

  #cmake_print_variables(PROJECT_SOURCE_DIR)
  #cmake_print_variables(CMAKE_CURRENT_LIST_DIR)

  target_include_directories(${lib_name} ${WARNING_GUARD} INTERFACE 
                                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR})
endfunction()


add_lib_interface(urc)
add_library(urc::urc ALIAS urc)

#urc library uses c++ 20 which is set via myproject::myproject_options
# target_compile_features(urc PUBLIC cxx_std_20)


#generate_export_header(unique_rc EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/unique_rc/include/unique_rc_export.hpp)
