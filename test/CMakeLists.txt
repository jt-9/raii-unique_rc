cmake_minimum_required(VERSION 3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(myproject CONFIG REQUIRED) # for sample_app, project_options, ...

  if(NOT TARGET myproject_options)
    message(FATAL_ERROR "Required config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

# ---- Dependencies ----

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

# Provide a simple smoke test to make sure that the CLI works and can display a --help message
add_test(NAME cli.has_help COMMAND sample_app --help)

# Provide a test to verify that the version being reported from the application
# matches the version given to CMake. This will be important once you package
# your program. Real world shows that this is the kind of simple mistake that is easy
# to make, but also easy to test for.
# add_test(NAME cli.version_matches COMMAND sample_app --version)
# set_tests_properties(cli.version_matches PROPERTIES PASS_REGULAR_EXPRESSION "${PROJECT_VERSION}")

set(TEST_HEADER_PREFIX inc)
set(TEST_SOURCE_PREFIX src)

set(TESTS_HEADERS 
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_HEADER_PREFIX}/mock_pointer_no_op.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_HEADER_PREFIX}/mock_generic_deallocator_no_op.hpp
)

set(TESTS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/unique_ptr_tests.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/assignment/forward_deleter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/assignment/deleter_inheritance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/assignment/move_assign_neg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/assignment/move_array.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/assignment/move_single.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/assignment/nullptr.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/construction/construct_default.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/construction/construct_move.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/construction/construct_move_neg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/construction/cv_qualifiers.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/construction/from_pointer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/construction/from_pointer_array.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/construction/lwg2905.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/construction/noexcept_construct.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/construction/nullptr.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/creation/aggregate.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/creation/array.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/creation/for_overwrite.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/creation/single.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/hash/hash.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/hash/types.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/io/lwg2948.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/modifiers/constexpr_release.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/modifiers/constexpr_reset.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/modifiers/release.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/modifiers/reset.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/modifiers/cv_qualifiers.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/requirements/dr2228.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/requirements/dr2899.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/requirements/lwg2762.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/requirements/ptr_type_single.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/requirements/ptr_type_array.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/specialised_algorithms/comparisons.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/specialised_algorithms/swap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/specialised_algorithms/swap_incomplete_type.cpp

  $<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_SOURCE_DIR}/winapi_tests.cpp>
)

add_executable(tests ${TESTS_HEADERS} ${TESTS_SOURCES})
#target_compile_features(tests PUBLIC cxx_std_23)
#target_sources(tests PRIVATE $<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_SOURCE_DIR}/winapi_tests.cpp>)

target_link_libraries(
  tests
  PRIVATE myproject::myproject_warnings
          myproject::myproject_options
          myproject::urc
          Catch2::Catch2WithMain)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if (WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
    COMMAND_EXPAND_LISTS
  )
endif()

# Automatically discover tests that are defined in catch based test files you can modify the unittests.
# Set TEST_PREFIX to whatever you want, or use different for different binaries
catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)

set(CONSTEXPR_TESTS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/constexpr_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unique_ptr_tests.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/assignment/move_assign_neg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/construction/construct_move.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/modifiers/constexpr_release.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE_PREFIX}/modifiers/constexpr_reset.cpp
)

  # Add a file containing a set of constexpr tests
add_executable(constexpr_tests ${TESTS_HEADERS} ${CONSTEXPR_TESTS_SOURCES})
#target_compile_features(constexpr_tests PUBLIC cxx_std_23)

target_link_libraries(
  constexpr_tests
  PRIVATE myproject::myproject_warnings
          myproject::myproject_options
          myproject::urc
          Catch2::Catch2WithMain)

target_include_directories(constexpr_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

catch_discover_tests(
  constexpr_tests
  TEST_PREFIX
  "constexpr."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "constexpr."
  OUTPUT_SUFFIX
  .xml)

# Disable the constexpr portion of the test, and build again this allows us to have an executable that we can debug when
# things go wrong with the constexpr testing
add_executable(relaxed_constexpr_tests ${TESTS_HEADERS} constexpr_tests.cpp)
target_link_libraries(
  relaxed_constexpr_tests
  PRIVATE myproject::myproject_warnings
          myproject::myproject_options
          myproject::urc
          Catch2::Catch2WithMain)
target_compile_definitions(relaxed_constexpr_tests PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
target_include_directories(relaxed_constexpr_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

catch_discover_tests(
  relaxed_constexpr_tests
  TEST_PREFIX
  "relaxed_constexpr."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "relaxed_constexpr."
  OUTPUT_SUFFIX
  .xml)
