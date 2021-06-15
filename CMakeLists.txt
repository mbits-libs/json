cmake_minimum_required (VERSION 3.10)
project (mstch-json
  VERSION 0.1.0
  DESCRIPTION "JSON implemented in terms of no1msd's libmstch."
  LANGUAGES CXX)

set(MSTCH_TESTING ON CACHE BOOL "Compile and/or run self-tests")

set(PROJECT_VERSION_SHORT "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
set(PROJECT_VERSION_STABILITY "")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

if (NOT CMAKE_CXX_STANDARD)
    if (MSTCH_TESTING)
        # Google test uses result_of
        set(CMAKE_CXX_STANDARD 17)
    else()
        set(CMAKE_CXX_STANDARD 20)
    endif()
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if (MSTCH_TESTING)
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

add_subdirectory(external)

add_library(mstch-json STATIC
    src/mstch-json.cpp
    include/mstch/mstch-json.hpp)
set_target_properties(mstch-json PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR})
target_include_directories(mstch-json PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(mstch-json PRIVATE ${ADDITIONAL_WALL_FLAGS})
target_link_libraries(mstch-json PUBLIC mstch)

##################################################################
##  TESTING
##################################################################

if (MSTCH_TESTING)
set(COVERALLS_PREFIX MSTCH_)
set(MSTCH_COVERALLS_DIRS src include)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
include(tools/coveralls/Coveralls.cmake)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tests)

file(GLOB TEST_SRCS tests/*.cc)

add_executable(mstch-json-test ${TEST_SRCS})
target_compile_options(mstch-json-test PRIVATE ${ADDITIONAL_WALL_FLAGS})
target_link_libraries(mstch-json-test mstch-json GTest::gtest_main)

add_test(NAME mstch-json COMMAND mstch-json-test)

endif()
