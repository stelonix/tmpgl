cmake_minimum_required(VERSION 2.6)
 
SET(PROJECTNAME gl3)
 
PROJECT(${PROJECTNAME})
 
set(main_SRCS
      helpers.cpp
      boilerplate.cpp
      main.cpp
      unwind.cpp
      png_texture.cpp
   )
 
SET(LIBRARIES ${LIBRARIES})

# libpng
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

# OpenGL
FIND_PACKAGE(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Libunwind REQUIRED)
if ( LIBUNWIND_FOUND )
	include_directories( ${LIBUNWIND_INCLUDE_DIRS} )
endif(LIBUNWIND_FOUND)

find_package(GLEW REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

SET(LIBRARIES ${LIBRARIES} ${OPENGL_LIBRARIES}
	${PNG_LIBRARY} ${UNWIND_SHARED_LIB} ${GLEW_LIBRARY})
 
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} )
 
ADD_EXECUTABLE(${PROJECTNAME} ${main_SRCS})

set(CMAKE_CXX_FLAGS "-std=c++11")
 
TARGET_LINK_LIBRARIES(${PROJECTNAME} ${LIBRARIES})