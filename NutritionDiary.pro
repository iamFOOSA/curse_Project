QT += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = NutritionDiary
TEMPLATE = app

SOURCES += \
    sources/main.cpp \
    sources/mainwindow.cpp \
    sources/registrationwindow.cpp \
    sources/menuwindow.cpp \
    sources/trackingwindow.cpp \
    sources/summarywindow.cpp \
    sources/food.cpp \
    sources/user.cpp \
    sources/manager.cpp \
    sources/recommendations.cpp \
    sources/nutrition_manager.cpp \
    sources/nutrition_advisor.cpp \
    sources/meal_tracker.cpp \
    sources/food_file_manager.cpp \
    sources/history_manager.cpp \
    sources/trend_analyzer.cpp

HEADERS += \
    headers/mainwindow.h \
    headers/registrationwindow.h \
    headers/menuwindow.h \
    headers/trackingwindow.h \
    headers/summarywindow.h \
    headers/manager.h \
    headers/recommendations.h \
    headers/food.h \
    headers/user.h \
    headers/nutrition_manager.h \
    headers/nutrition_advisor.h \
    headers/meal_tracker.h \
    headers/food_file_manager.h \
    headers/history_manager.h \
    headers/trend_analyzer.h

FORMS += \
    mainwindow.ui

DEFINES += QT_USE_QSTRINGBUILDER

CONFIG += debug_and_release
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/build/debug
    OBJECTS_DIR = $$PWD/build/debug/obj
    MOC_DIR = $$PWD/build/debug/moc
    RCC_DIR = $$PWD/build/debug/rcc
    UI_DIR = $$PWD/build/debug/ui
} else {
    DESTDIR = $$PWD/build/release
    OBJECTS_DIR = $$PWD/build/release/obj
    MOC_DIR = $$PWD/build/release/moc
    RCC_DIR = $$PWD/build/release/rcc
    UI_DIR = $$PWD/build/release/ui
}

QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS += -Wall -Wextra

win32 {
    RC_ICONS = icon.ico
}

macx {
    ICON = icon.icns
}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

system(mkdir -p data)
system(mkdir -p exports)
