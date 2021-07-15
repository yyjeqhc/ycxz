QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    control.cpp \
    helper.cpp \
    main.cpp \
    dialog.cpp \
    normal.cpp \
    screen.cpp \
    showscreen.cpp \
    student.cpp \
    tcpserver.cpp \
    tcpsocket.cpp \
    teacher.cpp

HEADERS += \
    control.h \
    dialog.h \
    helper.h \
    normal.h \
    screen.h \
    showscreen.h \
    student.h \
    tcpserver.h \
    tcpsocket.h \
    teacher.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/../../qt_qqyxh/untitled4/ -luser32

INCLUDEPATH += $$PWD/../../qt_qqyxh/untitled4
DEPENDPATH += $$PWD/../../qt_qqyxh/untitled4

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../qt_qqyxh/untitled4/user32.lib
