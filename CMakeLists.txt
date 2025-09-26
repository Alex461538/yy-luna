cmake_minimum_required(VERSION 3.10.0)
project(yy-luna VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_executable(yy-luna 
    src/main.cpp
)

target_include_directories(yy-luna 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Linking examples, you could make bindings for SDL
# target_link_libraries(yy-luna glfw)
# target_link_libraries(yy-luna openal) # libopenal-dev
# target_link_libraries(yy-luna sndfile) # 