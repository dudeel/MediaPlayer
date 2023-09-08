cmake_minimum_required(VERSION 2.8)
project(MediaPlayer)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt5GStreamer REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

file(GLOB_RECURSE SRC_FILES *.cpp *.h *.ui)


add_executable(${PROJECT_NAME} main.cpp media-player-window.cpp media-player-window.h media-player-window.ui)
target_link_libraries(${PROJECT_NAME} ${QTGSTREAMER_UI_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Widgets
    ${GST_LIBRARIES}
    ${GST_LIBRARY_DIRS}
    ${Qt5GStreamer_LIBRARIES}
)
