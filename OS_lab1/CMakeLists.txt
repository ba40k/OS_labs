cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)
#SHARED - dynamic library, STATIC - static library
add_library(MyLib SHARED
    src/Number.cpp
    src/Vector.cpp
)

add_executable(MyProject src/main.cpp)
target_link_libraries(MyProject PRIVATE MyLib)
