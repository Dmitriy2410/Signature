TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/CommonUtils.cpp \
        src/main.cpp

HEADERS += \
    src/CommonUtils.h \
    src/SignatureParams.h

INCLUDEPATH += src/
