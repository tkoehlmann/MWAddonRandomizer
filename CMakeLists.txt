cmake_minimum_required(VERSION 2.6)
project(morrowindaddonrandomizercpp)
set(PROJECT_VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
add_executable(morrowindaddonrandomizercpp
    main.cpp
    esmtools/esmloader.cpp
    esmtools/esmwriter.cpp
    records.cpp
    iohelpers.cpp
    settings.cpp
    randomizer.cpp
    randomizers/weapons.cpp
)

add_library(mwaddonrandomizer SHARED records.cpp iohelpers.cpp)
set_target_properties(mwaddonrandomizer PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(mwaddonrandomizer PROPERTIES SOVERSION 0)

