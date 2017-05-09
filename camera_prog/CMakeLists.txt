 cmake_minimum_required(VERSION 2.8)
project( Camera_test )

find_package(raspicam REQUIRED)
find_package( OpenCV REQUIRED )
find_package(PkgConfig)

pkg_check_modules(PC_ZeroMQ QUIET zmq)

find_path(ZeroMQ_INCLUDE_DIR
		NAMES zmq.hpp
		PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
		)

find_library(ZeroMQ_LIBRARY
		NAMES zmq
		PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
		)

IF  ( OpenCV_FOUND AND raspicam_CV_FOUND)
MESSAGE(STATUS "COMPILING OPENCV TESTS")
add_executable( Camera_test hinderdet_main.cpp hinder_det.cpp)
set_property(TARGET Camera_test PROPERTY CXX_STANDARD 11)
target_link_libraries( Camera_test ${raspicam_CV_LIBS} ${ZeroMQ_LIBRARY})
ELSE()
MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
ENDIF()