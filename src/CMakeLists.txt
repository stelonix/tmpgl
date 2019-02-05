cmake_minimum_required(VERSION 2.6)

SET(PROJECTNAME gl3)
SET(CMAKE_C_COMPILER /usr/bin/gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
PROJECT(${PROJECTNAME})
cmake_policy(SET CMP0072 NEW)
set(main_SRCS
		include/gl3w.c
		include/imgui/imgui.cpp
		include/imgui/imgui_draw.cpp
		include/imgui/imgui_widgets.cpp
		include/imgui/imgui_impl_glfw.cpp
		include/imgui/imgui_impl_opengl3.cpp
		include/imgui/imgui_demo.cpp
		engine/texture_manager.cpp
		engine/atlas/texture_atlas.cpp
		engine/atlas/atlas_builder.cpp
		#include/imgui/imgui_draw.cpp
		#include/imgui/imgui.cpp
		loader/loader.cpp
		engine/rt/eng_sprite.cpp
		data_transform.cpp
		helpers/util.cpp
		cfg.cpp
		engine/vbo.cpp
		logging.cpp
		engine/engine.cpp
		engine/text/text_engine.cpp
		engine/text/fonts.cpp
		helpers/generators.cpp
		png_texture.cpp
		engine/rt/eng_texture.cpp
		loader/assets.cpp
		engine/datatypes/game_map.cpp
		engine/datatypes/game_sprite.cpp
		engine/datatypes/game_tileset.cpp
		helpers/helpers.cpp
		engine/shader.cpp
		#boilerplate.cpp
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
find_package(GLFW3 REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Libunwind REQUIRED)
if ( LIBUNWIND_FOUND )
	include_directories( ${LIBUNWIND_INCLUDE_DIRS} )
endif(LIBUNWIND_FOUND)

include_directories( "${CMAKE_SOURCE_DIR}/include/" )

#	find_package(GLEW REQUIRED)
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
	${GLFW3_LIBRARY}
	${OPENGL_LIBRARIES}
	${PNG_LIBRARIES}
	${UNWIND_SHARED_LIB}
	${X11_LIBRARIES} ${CMAKE_DL_LIBS}
	)

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} )
include_directories(${CMAKE_SOURCE_DIR})
ADD_EXECUTABLE(${PROJECTNAME} ${main_SRCS})

set(CMAKE_CXX_FLAGS "-std=c++14 -g -pthread")

TARGET_LINK_LIBRARIES(${PROJECTNAME} ${LIBRARIES})
