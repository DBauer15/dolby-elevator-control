cmake_minimum_required(VERSION 3.16)

project(ec)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES 
    "src/*.cpp"
)

add_executable(${CMAKE_PROJECT_NAME}
    ${SOURCES}
)