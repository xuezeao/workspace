#-------------------------------------------------
#
# Project created by QtCreator 2017-01-31T13:40:43
#
#-------------------------------------------------

QT       += core gui
QT       += core sql

QT       += network
QT       += serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartCabinetV0_0_1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainchoice.cpp \
    checkboxdelegate.cpp \
    global_Vailable.cpp \
    httpgp.cpp \
    myqheaderview.cpp \
    pbtdelegate.cpp \
    readstminfo.cpp \
    uart4stm.cpp \
    waitingpage.cpp \
    delaytime.cpp \
    dialog_controlthread.cpp \
    seriportquery.cpp \
    wthread.cpp \
    httptest.cpp \
    signinui.cpp

HEADERS  += mainchoice.h \
    checkboxdelegate.h \
    global_Vailable.h \
    httpgp.h \
    myqheaderview.h \
    pbtdelegate.h \
    readstminfo.h \
    Sql_Setting.h \
    uart4stm.h \
    waitingpage.h \
    delaytime.h \
    dialog_controlthread.h \
    seriportquery.h \
    wthread.h \
    httptest.h \
    signinui.h

FORMS    += mainchoice.ui \
    waitingpage.ui \
    dialog_controlthread.ui

DISTFILES += \
    resource/loading.gif
