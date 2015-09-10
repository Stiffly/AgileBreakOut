project(daydream)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem thread chrono)
find_package(assimp REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(BGFX REQUIRED)
find_package(liquidfun REQUIRED)
# GLM
if(UNIX)
    find_package(X11 REQUIRED)
endif()

set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)
include_directories(
    ${INCLUDE_PATH}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${assimp_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${BGFX_INCLUDE_DIRS}
	${liquidfun_INCLUDE_DIRS}
)

file(GLOB SOURCE_FILES_Core
    "${INCLUDE_PATH}/Core/*.h"
    "Core/*.cpp"
)
file(GLOB SOURCE_FILES_Core_Util
    "${INCLUDE_PATH}/Core/Util/*.h"
    "Core/Util/*.cpp"
)
source_group(Core FILES ${SOURCE_FILES_Core})
source_group(Core\\Util FILES ${SOURCE_FILES_Core_Util})

file(GLOB SOURCE_FILES_Input
    "${INCLUDE_PATH}/Input/*.h"
    "Input/*.cpp"
)
source_group(Input FILES ${SOURCE_FILES_Input})

file(GLOB SOURCE_FILES_Rendering
    "${INCLUDE_PATH}/Rendering/*.h"
)
source_group(Rendering FILES ${SOURCE_FILES_Rendering})

file(GLOB SOURCE_FILES_Transform
    "${INCLUDE_PATH}/Transform/*.h"
    "Transform/*.cpp"
)
source_group(Transform FILES ${SOURCE_FILES_Transform})

file(GLOB SOURCE_FILES_Game
    "${INCLUDE_PATH}/Game/*.h"
    "Game/*.cpp"
)
source_group(Game FILES ${SOURCE_FILES_Game})

set(SOURCE_FILES
    ${SOURCE_FILES_Core}
    ${SOURCE_FILES_Core_Util}
    ${SOURCE_FILES_Input}
    ${SOURCE_FILES_Rendering}
    ${SOURCE_FILES_Transform}
    ${SOURCE_FILES_Game}
        Physics/PhysicsSystem.cpp ../../include/Physics/PhysicsSystem.h ../../include/Core/CBoxShape.h ../../include/Physics/CPhysics.h ../../include/Physics/EContact.h)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

add_definitions(-DBGFX_CONFIG_RENDERER_OPENGL=33)

add_library(game ${SOURCE_FILES})
target_link_libraries(game
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${Boost_LIBRARIES}
    ${assimp_LIBRARIES}
    ${PNG_LIBRARIES}
    ${X11_LIBRARIES}
    ${BGFX_LIBRARIES}
	${liquidfun_LIBRARIES}
)

add_executable(breakout 
	main.cpp
)
target_link_libraries(breakout
	game
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${Boost_LIBRARIES}
    ${assimp_LIBRARIES}
    ${PNG_LIBRARIES}
	${X11_LIBRARIES}
	${BGFX_LIBRARIES}
    ${liquidfun_LIBRARIES}
)

