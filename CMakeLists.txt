cmake_minimum_required(VERSION 3.22)
project(jaledit VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Dependencies
###############################################################################

## raylib
set(RAYLIB_VERSION 4.2.2)
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

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/raygui_impl.cpp
    src/utils.cpp

    src/keybind/node.cpp
    src/keybind/trie.cpp

    src/rope/rope.cpp
    src/rope/node.cpp
    src/rope/node_leaf.cpp
    src/rope/node_branch.cpp
    src/rope/utils.cpp

    src/buffer.cpp
    src/editor.cpp
)

add_executable(test_rope
    src/rope/test.cpp

    src/rope/rope.cpp
    src/rope/node.cpp
    src/rope/node_leaf.cpp
    src/rope/node_branch.cpp
    src/rope/utils.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror -Wfatal-errors)

target_link_libraries(${PROJECT_NAME} raylib)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC external)
