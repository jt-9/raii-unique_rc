cmake_minimum_required(VERSION 3.30)

# Only set the cxx_standard if it is not set by someone else
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)    
endif()

set(CXX_STANDARD_REQUIRED True)

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)

project(
    UniqueRC 
    DESCRIPTION "Generic unique RAII lib"
    LANGUAGES CXX
)

include(cmake/PreventInSourceBuilds.cmake)
include(ProjectOptions.cmake)

urc_setup_options()

urc_global_options()

include(Dependencies.cmake)
urc_setup_dependencies()

urc_local_options()

target_compile_features(urc_project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_library(urc::project_options ALIAS urc_project_options)
add_library(urc::project_warnings ALIAS urc_project_warnings)

# configure files based on CMake configuration options
add_subdirectory(configured_files)

# Adding the src:
add_subdirectory(unique_rc)

if(urc_BUILD_EXAMPLE)
  add_subdirectory(examples)
endif()

# Don't even look at tests if we're not top level
if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()


# Adding the tests:
include(CTest)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

# if(urc_BUILD_FUZZ_TESTS)
#   message(AUTHOR_WARNING "Building Fuzz Tests, using fuzzing sanitizer https://www.llvm.org/docs/LibFuzzer.html")
#   if (NOT urc_ENABLE_SANITIZER_ADDRESS AND NOT urc_ENABLE_SANITIZER_THREAD)
#     message(WARNING "You need asan or tsan enabled for meaningful fuzz testing")
#   endif()
#   add_subdirectory(fuzz_test)

# endif()

# If MSVC is being used, and ASAN is enabled, we need to set the debugger environment
# so that it behaves well with MSVC's debugger, and we can run the target from visual studio
if(MSVC)
  get_all_installable_targets(all_targets)
  message("all_targets=${all_targets}")
  set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()

# set the startup project for the "play" button in MSVC
if (urc_BUILD_EXAMPLE AND MSVC)
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT demo_unique_rc)
endif()
