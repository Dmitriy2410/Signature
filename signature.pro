TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/CommonUtils.cpp \
        src/MD5.cpp \
        src/SignatureProcess.cpp \
        src/main.cpp

HEADERS += \
    src/CommonUtils.h \
    src/MD5.h \
    src/SignatureParams.h \
    src/SignatureProcess.h

INCLUDEPATH += src/
