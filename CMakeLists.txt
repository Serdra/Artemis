cmake_minimum_required(VERSION 3.0.0)
project(Rotom VERSION 0.1.0)

#  Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG -ffast-math -flto -std=c++2a")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

include(CTest)
enable_testing()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(Artemis main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
