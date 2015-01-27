cmake_minimum_required(VERSION 2.8.4)
project(fow)

include_directories(external/sdl/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB SOURCE_FILES src/*.cpp)
link_directories(libs)

add_executable(fow ${SOURCE_FILES})

target_link_libraries(fow SDL2_ttf.a)
target_link_libraries(fow SDL2_image.a)
target_link_libraries(fow SDL2main.lib)
target_link_libraries(fow SDL2.lib)
target_link_libraries(fow freetype)
target_link_libraries(fow glew32s.lib)
target_link_libraries(fow opengl32)
