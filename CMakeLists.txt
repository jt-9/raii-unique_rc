cmake_minimum_required(VERSION 3.23)

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
    VERSION 1.0.1
    DESCRIPTION "Unique RAII abstraction"
    LANGUAGES CXX
)

include(cmake/PreventInSourceBuilds.cmake)
include(ProjectOptions.cmake)

myproject_setup_options()

myproject_global_options()

include(Dependencies.cmake)
myproject_setup_dependencies()

myproject_local_options()

target_compile_features(myproject_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_library(myproject::myproject_options ALIAS myproject_options)
add_library(myproject::myproject_warnings ALIAS myproject_warnings)

# configure files based on CMake configuration options
add_subdirectory(configured_files)

# Adding the src:
add_subdirectory(unique_rc)
add_subdirectory(sample_app)

# Adding the tests:
include(CTest)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

# if(myproject_BUILD_FUZZ_TESTS)
#   message(AUTHOR_WARNING "Building Fuzz Tests, using fuzzing sanitizer https://www.llvm.org/docs/LibFuzzer.html")
#   if (NOT myproject_ENABLE_ADDRESS_SANITIZER AND NOT myproject_ENABLE_THREAD_SANITIZER)
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
if (MSVC)
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT sample_app)
endif()