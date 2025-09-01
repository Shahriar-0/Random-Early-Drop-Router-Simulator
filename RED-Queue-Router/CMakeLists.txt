cmake_minimum_required(VERSION 3.16)
project(RED-Queue-Router VERSION 1.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(RED-Queue-Router
    main.cpp
    event.h
    metrics.cpp
    metrics.h
    packet.h
    packet_generator.cpp
    packet_generator.h
    queue.cpp
    queue.h
    router.cpp
    router.h
    simulator.cpp
    simulator.h
)

target_link_libraries(RED-Queue-Router
    Qt${QT_VERSION_MAJOR}::Core
)

include(GNUInstallDirs)
install(TARGETS RED-Queue-Router
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
