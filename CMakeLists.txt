cmake_minimum_required(VERSION 3.22.1)
project(chesscpp)

option(BUILD_TESTS "Activar tests?" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE
    "-O3 -Ofast -march=native -mtune=native -flto -fno-math-errno -fno-trapping-math -ffast-math -funroll-loops -fomit-frame-pointer"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

file(GLOB SRC_FILES
    "src/*.cpp"
)

set(INC_FILES_DIR
    include/
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()


include_directories(
    ${INC_FILES_DIR}
)

add_executable(chesscpp
    ${SRC_FILES}
)

target_link_options(chesscpp PRIVATE "LINKER:-O3,-s")
