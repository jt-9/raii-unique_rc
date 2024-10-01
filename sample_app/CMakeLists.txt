#include(CMakePrintHelpers)

set(SAMPLE_APP_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/inc/test_deleter.hpp
)

set(SAMPLE_APP_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

#cmake_print_variables(SAMPLE_APP_HEADERS SAMPLE_APP_SOURCES)

set(EXEC_TARGET unique_rc_sample)

add_executable(${EXEC_TARGET} ${SAMPLE_APP_HEADERS} ${SAMPLE_APP_SOURCES})

target_link_libraries(${EXEC_TARGET} PRIVATE 
                  myproject::myproject_options
                  myproject::myproject_warnings
                  myproject::urcd)

target_link_system_libraries(${EXEC_TARGET} PRIVATE 
                                                      CLI11::CLI11
                                                      fmt::fmt
                                                      spdlog::spdlog)

target_include_directories(${EXEC_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc ${CMAKE_BINARY_DIR}/configured_files/include)