cmake_minimum_required(VERSION 3.10.0)
project(glin VERSION 0.1.0 LANGUAGES C CXX)

set(SOURCES main.cpp src/cli.cpp src/lexer.cpp src/token.cpp src/paths.cpp)
add_executable(glin ${SOURCES})

target_include_directories(glin PRIVATE src)
target_include_directories(glin PUBLIC include)
target_compile_features(glin PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(argh CONFIG REQUIRED)
target_link_libraries(glin PRIVATE argh)

find_package(semver CONFIG REQUIRED)
target_link_libraries(glin PRIVATE semver::semver)