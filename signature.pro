TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

DESTDIR = bin

SOURCES += \
        src/CommonUtils.cpp \
        src/DataBufferStorage.cpp \
        src/MD5.cpp \
        src/SignatureAlgorithm.cpp \
        src/main.cpp

HEADERS += \
    src/CommonUtils.h \
    src/DataBufferStorage.h \
    src/MD5.h \
    src/SignatureAlgorithm.h \
    src/SignatureParams.h

INCLUDEPATH += src/

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread
