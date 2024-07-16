cmake_minimum_required(VERSION 3.20)
project(vanche_3d VERSION 0.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

if((${CMAKE_CXX_COMPILER_ID} STREQUAL GNU) OR (${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang) OR (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang))
  message(STATUS "${CMAKE_CXX_COMPILER_ID} compiler detected, adding compile flags")
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
  endif()
  set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -Wno-unused-function -Wno-missing-braces")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-g -O3")

  
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  message(STATUS "MSVC detected, adding compile flags")
  set(CMAKE_BUILD_TYPE Debug)
  # set(CMAKE_CXX_FLAGS "-Wall")
  set(CMAKE_CXX_FLAGS_DEBUG "-DEBUG /Zi")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ox")

endif()

if(APPLE)
  include_directories("/usr/local/include") 
  link_directories("/usr/local/lib")
endif(APPLE)

add_subdirectory(extern)
add_subdirectory(src)