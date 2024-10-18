cmake_minimum_required(VERSION 3.30.4)
project(
	summation
	VERSION 1.0
	LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(
	summation src/environment.c src/expression.c src/summation.c src/main.c
)
target_include_directories(summation PRIVATE include)
target_link_libraries(summation PRIVATE m)
target_compile_options(
	summation
	PRIVATE -O2
			-Werror
			-Wall
			-Wextra
			-pedantic
			-Wfloat-equal
			-Wundef
			-Wshadow
			-Wpointer-arith
			-Wcast-align
			-Wstrict-prototypes
			-Wstrict-overflow=5
			-Wwrite-strings
			-Wcast-qual
			-Wconversion
			-Wunreachable-code
			-fsanitize=address
)
target_link_options(summation PRIVATE -fsanitize=address)

if(UNIT_TESTING)
	list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmocka)

	find_package(cmocka REQUIRED)

	include(AddCMockaTest)

	add_subdirectory(test)
endif(UNIT_TESTING)
