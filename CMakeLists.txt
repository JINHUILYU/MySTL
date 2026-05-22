cmake_minimum_required(VERSION 3.14)
project(MySTL VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Header-only library
add_library(mystl INTERFACE)
target_include_directories(mystl INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(mystl INTERFACE cxx_std_17)

option(MYSTL_BUILD_TESTS "Build unit tests" ON)
option(MYSTL_BUILD_EXAMPLES "Build examples" OFF)

if(MYSTL_BUILD_TESTS)
    enable_testing()
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.15.2
        )
        FetchContent_MakeAvailable(googletest)
    endif()
    add_subdirectory(tests)
endif()

if(MYSTL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
