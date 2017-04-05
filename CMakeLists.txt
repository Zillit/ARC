#The make file generator files. CMake will generate files for whatever enviorment it is installed in

#Minmimal version of cmake
cmake_minimum_required(VERSION 3.0)

#Project name
project(ARC)

#include dictornary will include all the headers
include_directories(include)
include_directories(include/TSBK07)

#src dictonary will include all source files. the GLOB command will add all files off 
#the type .cpp
file(GLOB SOURCES "src/*.cpp")
file(GLOB CSCOURCES "src/TSBK07/*.c")

#Find external packeges
IF (WIN32)
	FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
		$ENV{PROGRAMFILES}/GLEW/include
		${PROJECT_SOURCE_DIR}/src/nvgl/glew/include
		DOC "The directory where GL/glew.h resides")
	FIND_LIBRARY( GLEW_LIBRARY
		NAMES glew GLEW glew32 glew32s
		PATHS
		$ENV{PROGRAMFILES}/GLEW/lib
		${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
		${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
		DOC "The GLEW library")
ELSE (WIN32)
	FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where GL/glew.h resides")
	FIND_LIBRARY( GLEW_LIBRARY
		NAMES GLEW glew
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The GLEW library")
ENDIF (WIN32)
find_package(GLUT)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)
find_package(OpenGL)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)
find_package(glm)
include_directories(${glm_INCLUDE_DIRS})
link_directories(${glm_LIBRARY_DIRS})
add_definitions(${glm_DEFINITIONS})
if(NOT glm_FOUND)
    message(ERROR " GLM not found!")
endif(NOT glm_FOUND)

find_package(OpenCV)
IF  (NOT OpenCV_FOUND)
MESSAGE(FATAL_ERROR "RASPICAM NOT FOUND IN YOUR SYSTEM")
ENDIF()

find_package(raspicam )
IF  (NOT raspicam_CV_FOUND)
MESSAGE(FATAL_ERROR "RASPICAM NOT FOUND IN YOUR SYSTEM")
ENDIF()


#creates a exe file
add_executable(ARC ${SOURCES} ${CSOURCES})
target_compile_features(ARC PRIVATE cxx_range_for)


target_link_libraries(ARC ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${glm_LIBRARY} ${GLEW_LIBRARY} ${OpenCV_LIBS} ${raspicam_CV_LIBS})

