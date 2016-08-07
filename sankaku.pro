TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lcurl -lpthread

SOURCES += main.cpp \
    network.cpp \
    networkworker.cpp \
    tags.cpp \
    request.cpp \
    utility.cpp \
    SankakuChannel/context.cpp \
    SankakuChannel/imagedownloader.cpp \
    SankakuChannel/query.cpp \
    SankakuChannel/postdownloader.cpp \
    terminalapi.cpp

HEADERS += \
    network.h \
    networkworker.h \
    tags.h \
    request.h \
    utility.h \
    SankakuChannel/context.h \
    SankakuChannel/imagedownloader.h \
    SankakuChannel/query.h \
    SankakuChannel/postdownloader.h \
    config.h \
    terminalapi.h
