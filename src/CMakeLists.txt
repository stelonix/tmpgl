cmake_minimum_required(VERSION 2.6)
 
SET(PROJECTNAME gl3)
SET(CMAKE_C_COMPILER /usr/bin/gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
PROJECT(${PROJECTNAME})

set(main_SRCS
		cfg.cpp
		vbo.cpp
		engine.cpp
		text_engine.cpp
		fonts.cpp
		generators.cpp
		png_texture.cpp
		eng_texture.cpp
		assets.cpp
		game_map.cpp
		game_sprite.cpp
		game_tileset.cpp
		helpers.cpp
		shader.cpp
		boilerplate.cpp
		main.cpp
		debug.cpp
		
   )
 
SET(LIBRARIES ${LIBRARIES})

# libpng
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

# OpenGL
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Libunwind REQUIRED)
if ( LIBUNWIND_FOUND )
	include_directories( ${LIBUNWIND_INCLUDE_DIRS} )
endif(LIBUNWIND_FOUND)

find_package(GLEW REQUIRED)
find_package(X11 REQUIRED)
# Freetype
find_package(Freetype REQUIRED)
	include_directories(${FREETYPE_INCLUDE_DIRS})
find_package(HarfBuzz REQUIRED)
	include_directories(${HARFBUZZ_INCLUDE_DIRS})

find_package(Cairo REQUIRED)
 	include_directories(${CAIRO_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

SET(LIBRARIES ${LIBRARIES} ${FREETYPE_LIBRARIES}
	${CAIRO_LIBRARIES}
	${HARFBUZZ_LIBRARIES}
	${OPENGL_LIBRARIES} ${GLEW_LIBRARY}
	${PNG_LIBRARIES}
	${UNWIND_SHARED_LIB} 
	${X11_LIBRARIES}
	)
 
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} )
 
ADD_EXECUTABLE(${PROJECTNAME} ${main_SRCS})

set(CMAKE_CXX_FLAGS "-std=c++11")
 
TARGET_LINK_LIBRARIES(${PROJECTNAME} ${LIBRARIES})