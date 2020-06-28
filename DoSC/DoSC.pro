QT += core
QT -= gui
CONFIG += c++11

TARGET = DoSC
TEMPLATE = lib
VERSION = 0.0.1

DEFINES += DOSC_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        can.cpp\
        dosc.cpp \
    isotp.cpp \
    isotpsession.cpp \
    resettabletimer.cpp \
    dosc_obd2.cpp

HEADERS += \
        can.h \
        dosc.h \
        dosc_global.h \
    isotp.h \
    isotpsession.h \
    resettabletimer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
