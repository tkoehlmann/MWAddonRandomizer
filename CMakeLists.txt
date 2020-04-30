cmake_minimum_required(VERSION 2.6)
project(morrowindaddonrandomizercpp)
set(PROJECT_VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -ggdb -Wall -Wextra")
add_executable(morrowindaddonrandomizercpp
    src/main.cpp
    src/esmtools/esmloader.cpp
    src/esmtools/esmwriter.cpp
    src/records.cpp
    src/iohelpers.cpp
    src/settings.cpp
    src/randomizer.cpp
    src/globals/attributes.cpp
    src/globals/magiceffects.cpp
    src/globals/skills.cpp
    src/randomizers/weapons.cpp
    src/randomizers/alchemy.cpp
)

add_library(mwaddonrandomizer SHARED src/records.cpp src/iohelpers.cpp)
set_target_properties(mwaddonrandomizer PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(mwaddonrandomizer PROPERTIES SOVERSION 0)

