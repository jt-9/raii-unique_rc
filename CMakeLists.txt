cmake_minimum_required(VERSION 3.21)

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