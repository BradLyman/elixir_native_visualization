cmake_minimum_required(VERSION 3.9)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL2_BUILDING_LIBRARY true)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)

message("sdl found: ${SDL2_FOUND}")
message("sdl dir: ${SDL2_INCLUDE_DIRS}")
message("sdl libs: ${SDL2_LIBRARIES}'")
message("glew dir: ${GLEW_INCLUDE_DIRS}")
message("glew libs: ${GLEW_LIBRARIES}")
message("gl dir: ${OPENGL_INCLUDE_DIR}")
message("gl libs: ${OPNEGL_LIBRARIES}")

include_directories("./inc")
include_directories("/usr/lib/erlang/usr/include")
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++1z")

add_library(NatVis SHARED
    ./src/first_nif.cpp
    ./src/Context.cpp
    ./src/gl/GLException.cpp
    ./src/gl/Buffer.cpp)
target_link_libraries(NatVis ${SDL2_LIBRARIES} ${GLEW_LIBRARIES})

