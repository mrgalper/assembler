cmake_minimum_required(VERSION 3.0)
project(Assembler C)

# Set the output directory for the binary (in the project's root directory)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add the include directories
include_directories(.)
include_directories(include/)
include_directories(utils/include/)

# Gather the source files
file(GLOB SOURCES src/*.c utils/src/*.c)

# Create the executable (assembler binary) in the root of the project directory
add_executable(assembler assembler.c ${SOURCES})

# Set compiler flags: pedantic, ansi, Wall
target_compile_options(assembler PRIVATE -pedantic -ansi -Wall)
set_target_properties(assembler PROPERTIES C_STANDARD 90)

# Move the assembler binary to the project's root directory after building
add_custom_command(
    TARGET assembler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:assembler> ${CMAKE_CURRENT_SOURCE_DIR}
)

