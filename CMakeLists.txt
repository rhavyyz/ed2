cmake_minimum_required(VERSION 3.12)
project(b)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)
cmake_policy(SET CMP0072 NEW)


add_executable(b ${PROJECT_SOURCE_DIR}/b.cpp )

set_property (TARGET b
  PROPERTY
    CXX_STANDARD 20
)

add_executable(min_heap ${PROJECT_SOURCE_DIR}/heap/min_heap.cpp )

set_property (TARGET min_heap
  PROPERTY
    CXX_STANDARD 20
)