cmake_minimum_required(VERSION 3.10)
project(ProcessLab_Windows)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
if(MSVC)
    add_compile_options(/W4 /O2)
else()
    add_compile_options(-Wall -Wextra -O2)
endif()

# killer executable
add_executable(killer killer.cpp)
if(WIN32)
    target_link_libraries(killer kernel32)
endif()

# user executable
add_executable(user user.cpp)
if(WIN32)
    target_link_libraries(user kernel32)
endif()

# victim executable
add_executable(victim victim.cpp)
if(WIN32)
    target_link_libraries(victim kernel32)
endif()

# Custom target for clean
if(MSVC)
    add_custom_target(clean_all
        COMMAND ${CMAKE_COMMAND} -E remove killer.exe user.exe victim.exe
        COMMENT "Cleaning executables"
    )
endif()
