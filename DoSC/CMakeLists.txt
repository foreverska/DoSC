cmake_minimum_required(VERSION 3.9)

project (DoSC)
add_compile_definitions(NO_QT)
include_directories(.)
FILE(GLOB SRC *.cpp) 
FILE(GLOB HEAD *.h)

add_library (DoSC SHARED ${SRC} ${HEAD})		
