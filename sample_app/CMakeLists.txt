#include(CMakePrintHelpers)

set(SAMPLE_APP_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/inc/test_deleter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/inc/consteval_lambda_example.hpp
)

set(SAMPLE_APP_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

#cmake_print_variables(SAMPLE_APP_HEADERS SAMPLE_APP_SOURCES)

set(EXEC_TARGET sample_app)

add_executable(${EXEC_TARGET} ${SAMPLE_APP_HEADERS} ${SAMPLE_APP_SOURCES})

# Request minimum c++ standard, default c++20
target_compile_features(${EXEC_TARGET} PUBLIC cxx_std_23)

target_link_libraries(${EXEC_TARGET} PRIVATE 
                  myproject::myproject_options
                  myproject::myproject_warnings
                  myproject::urc)

target_link_system_libraries(${EXEC_TARGET} PRIVATE 
                                            CLI11::CLI11
                                            fmt::fmt
                                            # spdlog::spdlog
                            )

target_include_directories(${EXEC_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc ${CMAKE_BINARY_DIR}/configured_files/include)
