cmake_minimum_required(VERSION 3.22.1)
project(chesscpp)

option(BUILD_TESTS "Activar tests?" OFF)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")
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







