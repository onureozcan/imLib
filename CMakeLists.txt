cmake_minimum_required(VERSION 3.14)
project(imageLib C)

set(CMAKE_C_STANDARD 11)
set(OPTIMIZATION_FLAG "-O0")
set(DEBUG_FLAG "-g")
set(CMAKE_C_FLAGS "-m32 -Wall ${DEBUG_FLAG}")

include_directories(src/include)
add_library(imageLib src/imlib.c src/ttf.c)

add_executable(main src/main.c)

target_link_libraries(main imageLib m)

set_target_properties(imageLib PROPERTIES COMPILE_FLAGS "-fno-stack-protector ${OPTIMIZATION_FLAG}")
set_target_properties(main PROPERTIES COMPILE_FLAGS "-m32 -masm=intel ${OPTIMIZATION_FLAG} -nostdlib ${DEBUG_FLAG} -Wall")