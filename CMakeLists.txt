cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0079 NEW)
project(NewMediaPlayer)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenCV REQUIRED)

find_package(Qt5GStreamer REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0)

include_directories(${GST_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS})
add_definitions(${GST_CFLAGS_OTHER})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${GSTREAMER_INCLUDE_DIRS})
include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Widgets
    ${GST_LIBRARIES}
    ${GSTREAMER_LIBRARIES}
    ${GST_LIBRARIES}
    ${GST_LIBRARY_DIRS}
    ${QtGStreamer_LIBRARIES}
    ${GSTREAMER_LIBRARIES}
    ${QTGSTREAMER_UI_LIBRARIES}
    ${OpenCV_LIBS}
    ${DARKNET_LIB}
)
