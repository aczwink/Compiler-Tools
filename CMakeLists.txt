cmake_minimum_required(VERSION 3.6)
project(Compiler-Tools)
find_package(XP-CMake REQUIRED) #we need advanced cmake modules

set(CMAKE_CXX_STANDARD 20)

XPC_AutoBuildType()
XPC_OptForHost()
XPC_SetCompileDefinitions()

find_package(StdPlusPlus REQUIRED)

add_subdirectory(lalr1gen)
add_subdirectory(lexgen)