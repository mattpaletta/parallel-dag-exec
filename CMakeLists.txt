cmake_minimum_required(VERSION 3.10)

project(parallel-dag-exec LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Enable coverage" OFF)
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behaviour Sanitizer" OFF)

## These two things are needed for YCM
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON )
IF(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
	EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
		${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
	)
ENDIF()

if (ENABLE_ASAN)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if (ENABLE_UBSAN)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
	set(CMAKE_CXX_FLAGS         "-Wall -Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused")
	set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

find_package(Threads REQUIRED)

file(GLOB INC_FILES include/*.hpp)
file(GLOB SRC_FILES lib/*.cpp)
add_library(dag STATIC ${INC_FILES} ${SRC_FILES})
target_include_directories(dag PUBLIC include)
target_link_libraries(dag Threads::Threads)

enable_testing()

find_package(Catch2 CONFIG REQUIRED)

add_executable(test_dag app/test_dag.cpp)
target_link_libraries(test_dag dag Catch2::Catch2)
add_test(dag test_dag)

add_executable(test_graph app/test_graph.cpp)
target_link_libraries(test_graph dag Catch2::Catch2)
add_test(graph test_graph)
