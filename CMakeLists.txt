cmake_minimum_required(VERSION 2.6)

include_directories(
    ${CMAKE_SOURCE_DIR}/include)

set(PROJECT http)

set(SOURCES main)

project(${PROJECT})

add_executable(${PROJECT}
    ${SOURCES})

target_link_libraries(${PROJECT}
cocaine-core
    cocaine-framework-native
boost_system
)

add_definitions(-std=c++0x -ftemplate-backtrace-limit=0)

file(COPY manifest.json DESTINATION .)
