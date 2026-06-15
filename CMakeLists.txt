cmake_minimum_required(VERSION 3.13)
project(codecrafters-shell)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

list(REMOVE_ITEM SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/run_program_win.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/run_program_unix.c"
)

if(WIN32)
    list(APPEND SOURCE_FILES src/modules/run_program_win.c)
elseif(UNIX)
    list(APPEND SOURCE_FILES src/modules/run_program_unix.c)
endif()

add_executable(shell ${SOURCE_FILES})

if(UNIX)
    target_link_libraries(shell PRIVATE readline)
endif()