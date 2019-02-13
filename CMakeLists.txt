cmake_minimum_required(VERSION 2.6)
project(morrowindaddonrandomizercpp)

add_executable(morrowindaddonrandomizercpp main.cpp)

install(TARGETS morrowindaddonrandomizercpp RUNTIME DESTINATION bin)
