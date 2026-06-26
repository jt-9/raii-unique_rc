#include(CMakePrintHelpers)


#cmake_print_variables(SAMPLE_APP_HEADERS SAMPLE_APP_SOURCES)


add_executable(demo_unique_rc)
target_sources(demo_unique_rc 
      PRIVATE UniquePtr.cpp
)

# Request minimum c++ standard, default c++20
target_compile_features(demo_unique_rc PUBLIC cxx_std_23)

target_link_libraries(demo_unique_rc 
              PRIVATE urc::project_options
                      urc::project_warnings
                      urc::urc
)

target_include_directories(demo_unique_rc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_executable(demo_coroutine)
target_sources(demo_coroutine 
      PRIVATE UniqueCoroutineHandle.cpp
)

# Request minimum c++ standard, default c++20
target_compile_features(demo_coroutine PUBLIC cxx_std_23)

target_link_libraries(demo_coroutine 
              PRIVATE urc::project_options
                      urc::project_warnings
                      urc::urc
)

target_include_directories(demo_coroutine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
