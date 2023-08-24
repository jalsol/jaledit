cmake_minimum_required(VERSION 3.22)
project(jaledit VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# Dependencies
###############################################################################

## raylib
set(RAYLIB_VERSION 4.2.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
        raylib
        URL https://github.com/jalsol/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

###############################################################################

include_directories(src)
include_directories(external/nativefiledialog/src/include)
link_directories(external/nativefiledialog/build/lib/Release/x64)

add_executable(${PROJECT_NAME}
    src/main.c
)

add_executable(vector_test
    src/common/vector.c
    src/common/vector_test.c
)

add_executable(rope_test
    src/common/vector.c

    src/rope/node.c
    src/rope/rope.c
    src/rope/test.c
)

add_executable(keybind_test
    src/keybind/node.c
    src/keybind/trie.c
    src/keybind/test.c
)

target_link_libraries(${PROJECT_NAME} raylib nfd)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC external)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror -Wfatal-errors)
