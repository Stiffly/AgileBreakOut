project(tests)

find_package(PNG REQUIRED)
find_package(Boost REQUIRED COMPONENTS chrono thread unit_test_framework)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${PNG_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)


file(GLOB SOURCE_FILES
    "*.cpp"
)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

add_executable(tests ${SOURCE_FILES})
target_link_libraries(tests
    game
    ${Boost_LIBRARIES}
)
