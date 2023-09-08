cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0079 NEW)
project(MediaPlayer)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PaATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt5GStreamer REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

add_subdirectory(src)

target_link_libraries(MediaPlayer PRIVATE ${GSTREAMER_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${QTGSTREAMER_UI_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Widgets
    ${GST_LIBRARIES}
    ${GST_LIBRARY_DIRS}
    ${Qt5GStreamer_LIBRARIES}
)
