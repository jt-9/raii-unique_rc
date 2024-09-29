set(SA_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)

set(EXEC_TARGET unique_rc_sample)

add_executable(${EXEC_TARGET} ${SA_SOURCES})

target_link_libraries(${EXEC_TARGET} PRIVATE myproject::myproject_options
                        myproject::myproject_warnings raii::urcd)

target_link_system_libraries(${EXEC_TARGET} PRIVATE 
                                                      CLI11::CLI11
                                                      fmt::fmt
                                                      spdlog::spdlog)

target_include_directories(${EXEC_TARGET} PRIVATE ${CMAKE_BINARY_DIR}/configured_files/include)