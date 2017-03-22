#The make file generator files. CMake will generate files for whatever enviorment it is installed in

#Minmimal version of cmake
cmake_minimum_required(VERSION 3.0)

#Project name
project(ARC)

#include dictornary will include all the headers
include_directories(include)

#src dictonary will include all source files. the GLOB command will add all files off 
#the type .cpp
file(GLOB SOURCES "src/*.cpp")

#Find external packeges
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

#creates a exe file
add_executable(ARC ${SOURCES})

target_link_libraries(ARC ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )