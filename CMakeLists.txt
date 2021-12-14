cmake_minimum_required (VERSION 3.10)
project (json
  VERSION 0.1.0
  DESCRIPTION "JSON implemented in terms of C++17 variant."
  LANGUAGES CXX)

set(JSON_TESTING ON CACHE BOOL "Compile and/or run self-tests")

set(PROJECT_VERSION_SHORT "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
set(PROJECT_VERSION_STABILITY "")

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
elseif(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  message(FATAL_ERROR "Missing file\n  ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake\nRun `conan install .. --build missing` before cmake")
endif()

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if (JSON_TESTING)
if (NOT TARGET GTest::gtest_main)
  find_package(GTest REQUIRED CONFIG HINTS ${PROJECT_BINARY_DIR})
endif()
endif()

if (MSVC)
  set(ADDITIONAL_WALL_FLAGS
      /permissive-
      /D_CRT_SECURE_NO_WARNINGS
      /Zc:__cplusplus
      /W4
      /w14242
      /w14254
      /w14263
      /w14265
      /w14287
      /we4289
      /w14296
      /w14311
      /w14545
      /w14546
      /w14547
      /w14549
      /w14555
      /w14619
      /w14640
      /w14826
      /w14905
      /w14906
      /w14928
      /w14946)
else()
  set(ADDITIONAL_WALL_FLAGS
      -Wall -Wextra
      -Wnon-virtual-dtor
      -Wold-style-cast
      -Wcast-align
      -Wunused
      -Woverloaded-virtual
      -Wpedantic
      -Wconversion
      -Wsign-conversion
      -Wnull-dereference
      -Wdouble-promotion
      -Wformat=2
  )
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    list(APPEND ADDITIONAL_WALL_FLAGS -fcolor-diagnostics) # -Wlifetime
  else()
    list(APPEND ADDITIONAL_WALL_FLAGS
      -fdiagnostics-color
      -Wmisleading-indentation
      -Wduplicated-cond
      -Wduplicated-branches
      -Wlogical-op
      -Wuseless-cast
      )
  endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(json STATIC
    src/json.cpp
    include/json/json.hpp
    include/json/serdes.hpp)
set_target_properties(json PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR})
target_include_directories(json PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(json PRIVATE ${ADDITIONAL_WALL_FLAGS})

if (MSVC)
	target_sources(json PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vs/u8string.natvis)
endif()

##################################################################
##  TESTING
##################################################################

if (JSON_TESTING)
set(COVERALLS_PREFIX JSON_)
set(JSON_COVERALLS_DIRS src include)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
include(tools/coveralls/Coveralls.cmake)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tests)

file(GLOB TEST_SRCS tests/*.cc)

add_executable(json-test ${TEST_SRCS})
target_compile_options(json-test PRIVATE ${ADDITIONAL_WALL_FLAGS})
target_link_libraries(json-test json GTest::gtest_main)

add_test(NAME json COMMAND json-test --gtest_output=xml:testing-results/json-${CMAKE_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE}.xml)

endif()
