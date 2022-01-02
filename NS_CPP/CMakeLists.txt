cmake_minimum_required(VERSION 3.0)
project(NSCPP)

include_directories(
    ${PROJECT_SOURCE_DIR}/NS
)

add_executable(NSCPP main.cpp NS/NSCPP.cpp)