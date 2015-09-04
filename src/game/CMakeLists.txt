project(daydream)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem thread chrono)
find_package(assimp REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
# GLM

set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include/dd)
include_directories(
    ${INCLUDE_PATH}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${assimp_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
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

file(GLOB SOURCE_FILES_Particles
    "${INCLUDE_PATH}/Particles/*.h"
    "Particles/*.cpp"
)
source_group(Particles FILES ${SOURCE_FILES_Particles})

file(GLOB SOURCE_FILES_Rendering
    "${INCLUDE_PATH}/Rendering/*.h"
)
source_group(Rendering FILES ${SOURCE_FILES_Rendering})

file(GLOB SOURCE_FILES_Timer
    "${INCLUDE_PATH}/Timer/*.h"
    "Timer/*.cpp"
)
source_group(Timer FILES ${SOURCE_FILES_Timer})

file(GLOB SOURCE_FILES_Transform
    "${INCLUDE_PATH}/Transform/*.h"
    "Transform/*.cpp"
)
source_group(Transform FILES ${SOURCE_FILES_Transform})

file(GLOB SOURCE_FILES_Trigger
    "${INCLUDE_PATH}/Trigger/*.h"
    "Trigger/*.cpp"
)
source_group(Trigger FILES ${SOURCE_FILES_Trigger})

set(SOURCE_FILES
    ${SOURCE_FILES_Core}
    ${SOURCE_FILES_Core_Util}
    ${SOURCE_FILES_Input}
    ${SOURCE_FILES_Particles}
    ${SOURCE_FILES_Rendering}
    ${SOURCE_FILES_Timer}
    ${SOURCE_FILES_Transform}
    ${SOURCE_FILES_Trigger}
)

add_library(daydream ${SOURCE_FILES})
target_link_libraries(daydream
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${Boost_LIBRARIES}
    ${assimp_LIBRARIES}
    ${PNG_LIBRARIES}
)