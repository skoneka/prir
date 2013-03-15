TEMPLATE = app
CONFIG += console
CONFIG -= qt

QMAKE_CFLAGS += -pthread
LIBS += -pthread

SOURCES += main.c
