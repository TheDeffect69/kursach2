cmake_minimum_required(VERSION 3.5)
project(UmlDeploymentDiagram VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Xml REQUIRED)

add_executable(UmlDeploymentDiagram
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(UmlDeploymentDiagram PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Xml
)
