include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


include(CheckCXXSourceCompiles)


macro(urc_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)

    message(STATUS "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform")
    set(TEST_PROGRAM "int main() { return 0; }")

    # Check if UndefinedBehaviorSanitizer works at link time
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)
    
    if(HAS_UBSAN_LINK_SUPPORT)
      message(STATUS "UndefinedBehaviorSanitizer is supported at both compile and link time.")
      set(SUPPORTS_UBSAN ON)
    else()
      message(WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
      set(SUPPORTS_UBSAN OFF)
    endif()
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    if (NOT WIN32)
      message(STATUS "Sanity checking AddressSanitizer, it should be supported on this platform")
      set(TEST_PROGRAM "int main() { return 0; }")

      # Check if AddressSanitizer works at link time
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
      check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)

      if(HAS_ASAN_LINK_SUPPORT)
        message(STATUS "AddressSanitizer is supported at both compile and link time.")
        set(SUPPORTS_ASAN ON)
      else()
        message(WARNING "AddressSanitizer is NOT supported at link time.")
        set(SUPPORTS_ASAN OFF)
      endif()
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(urc_setup_options)
  option(urc_ENABLE_HARDENING "Enable hardening" ON)
  option(urc_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    urc_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    urc_ENABLE_HARDENING
    OFF)

  urc_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR urc_PACKAGING_MAINTAINER_MODE)
    option(urc_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(urc_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(urc_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(urc_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(urc_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(urc_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(urc_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(urc_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(urc_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(urc_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(urc_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(urc_ENABLE_PCH "Enable precompiled headers" OFF)
    option(urc_ENABLE_CACHE "Enable ccache" OFF)
    option(urc_BUILD_EXAMPLE "Build example application" OFF)
  else()
    option(urc_ENABLE_IPO "Enable IPO/LTO" ON)
    option(urc_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(urc_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(urc_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(urc_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(urc_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(urc_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(urc_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(urc_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(urc_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(urc_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(urc_ENABLE_PCH "Enable precompiled headers" OFF)
    option(urc_ENABLE_CACHE "Enable ccache" ON)
    option(urc_BUILD_EXAMPLE "Build example application" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      urc_ENABLE_IPO
      urc_WARNINGS_AS_ERRORS
      urc_ENABLE_USER_LINKER
      urc_ENABLE_SANITIZER_ADDRESS
      urc_ENABLE_SANITIZER_LEAK
      urc_ENABLE_SANITIZER_UNDEFINED
      urc_ENABLE_SANITIZER_THREAD
      urc_ENABLE_SANITIZER_MEMORY
      urc_ENABLE_UNITY_BUILD
      urc_ENABLE_CLANG_TIDY
      urc_ENABLE_CPPCHECK
      urc_ENABLE_COVERAGE
      urc_ENABLE_PCH
      urc_ENABLE_CACHE
      )
  endif()

  urc_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (urc_ENABLE_SANITIZER_ADDRESS OR urc_ENABLE_SANITIZER_THREAD OR urc_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(urc_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(urc_global_options)
  if(urc_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    urc_enable_ipo()
  endif()

  urc_supports_sanitizers()

  if(urc_ENABLE_HARDENING AND urc_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR urc_ENABLE_SANITIZER_UNDEFINED
       OR urc_ENABLE_SANITIZER_ADDRESS
       OR urc_ENABLE_SANITIZER_THREAD
       OR urc_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${urc_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${urc_ENABLE_SANITIZER_UNDEFINED}")
    urc_enable_hardening(urc_project_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(urc_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(urc_project_warnings INTERFACE)
  add_library(urc_project_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  urc_set_project_warnings(
    urc_project_warnings
    ${urc_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(urc_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    urc_configure_linker(urc_project_options)
  endif()

  include(cmake/Sanitizers.cmake)
  urc_enable_sanitizers(
    urc_project_options
    ${urc_ENABLE_SANITIZER_ADDRESS}
    ${urc_ENABLE_SANITIZER_LEAK}
    ${urc_ENABLE_SANITIZER_UNDEFINED}
    ${urc_ENABLE_SANITIZER_THREAD}
    ${urc_ENABLE_SANITIZER_MEMORY})

  set_target_properties(urc_project_options PROPERTIES UNITY_BUILD ${urc_ENABLE_UNITY_BUILD})

  if(urc_ENABLE_PCH)
    target_precompile_headers(
      urc_project_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(urc_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    urc_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(urc_ENABLE_CLANG_TIDY)
    urc_enable_clang_tidy(urc_project_options ${urc_WARNINGS_AS_ERRORS})
  endif()

  if(urc_ENABLE_CPPCHECK)
    urc_enable_cppcheck(${urc_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(urc_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    urc_enable_coverage(urc_project_options)
  endif()

  if(urc_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(urc_project_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(urc_ENABLE_HARDENING AND NOT urc_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR urc_ENABLE_SANITIZER_UNDEFINED
       OR urc_ENABLE_SANITIZER_ADDRESS
       OR urc_ENABLE_SANITIZER_THREAD
       OR urc_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    urc_enable_hardening(urc_project_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
