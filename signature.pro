TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

DESTDIR = bin

SOURCES += \
        src/DataBufferStorage.cpp \
        src/Logger.cpp \
        src/MD5.cpp \
        src/OutputModule.cpp \
        src/SignatureAlgorithm.cpp \
        src/main.cpp

HEADERS += \
    src/DataBufferStorage.h \
    src/Logger.h \
    src/MD5.h \
    src/OutputModule.h \
    src/SignatureAlgorithm.h \
    src/SignatureParams.h

INCLUDEPATH += src/

QMAKE_CXXFLAGS += -pthread
LIBS += -pthread
