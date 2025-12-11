cmake_minimum_required(VERSION 3.16)

project(play VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

set(PROJECT_SOURCES
        sources/main.cpp
        sources/mainwindow.cpp
        headers/mainwindow.h
        mainwindow.ui
        sources/food.cpp
        headers/food.h
        sources/user.cpp
        headers/user.h
        headers/manager.h
        headers/recommendations.h
        headers/registrationwindow.h
        headers/menuwindow.h
        headers/trackingwindow.h
        " summarywindow..h"
        "headers/summarywindow.h"
        headers/summarywindow.h
        sources/manager.cpp
        sources/recommendations.cpp
        sources/registrationwindow.cpp
        sources/menuwindow.cpp
        sources/trackingwindow.cpp
        sources/summarywindow.cpp
        sources/recommendations.cpp
        headers/nutrition_manager.h
        headers/nutrition_advisor.h
        headers/meal_tracker.h
        sources/nutrition_manager.cpp
        sources/nutrition_advisor.cpp
        sources/meal_tracker.cpp
        headers/food_file_manager.h
        sources/food_file_manager.cpp
        headers/history_manager.h
        sources/history_manager.cpp
        headers/trend_analyzer.h
        sources/trend_analyzer.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(play
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET play APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(play SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(play
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(play PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)

# Добавляем include директории для заголовочных файлов
target_include_directories(play PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/sources
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.play)
endif()
set_target_properties(play PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS play
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(play)
    # Убеждаемся, что Charts включен в бандл
    if(APPLE)
        set_target_properties(play PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER com.example.play
            MACOSX_BUNDLE TRUE
        )
    endif()
endif()
