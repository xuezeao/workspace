#-------------------------------------------------
#
# Project created by QtCreator 2017-03-03T08:53:20
#
#-------------------------------------------------
QT       += core gui
QT       += core sql

QT       += network
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartCabinet
TEMPLATE = app


SOURCES += main.cpp \
    ui/common/dg_loginpage.cpp \
    ui/common/dg_functionoptpage.cpp \
    network/netcommunication.cpp \
    common/delaytime.cpp \
    common/initprocess.cpp \
    common/serialportcontrol.cpp \
    common/threadcontrol.cpp \
    common/timerandupdate.cpp \
    common/uart4stm.cpp \
    common/waitingpage.cpp \
    common/globalvariable.cpp \
    common/updatesql.cpp \
    common/commonmodel.cpp \
    ui/task/dg_returnpage.cpp \
    ui/exec/dg_exectakeout.cpp \
    ui/exec/dg_execreturn.cpp \
    ui/task/dg_takeoutpage.cpp \
    ui/task/dg_checkpage.cpp \
    ui/exec/dg_execcheck.cpp \
    ui/common/dg_requestposition.cpp \
    ui/task/dg_takein.cpp \
    ui/exec/dg_exectakein.cpp

HEADERS  += \
    ui/common/dg_loginpage.h \
    ui/common/dg_functionoptpage.h \
    common/globalvariable.h \
    network/netcommunication.h \
    common/delaytime.h \
    common/initprocess.h \
    common/serialportcontrol.h \
    common/Sql_Setting.h \
    common/threadcontrol.h \
    common/timerandupdate.h \
    common/uart4stm.h \
    common/waitingpage.h \
    common/updatesql.h \
    common/commonmodel.h \
    ui/task/dg_returnpage.h \
    ui/exec/dg_exectakeout.h \
    ui/exec/dg_execreturn.h \
    ui/task/dg_takeoutpage.h \
    ui/task/dg_checkpage.h \
    ui/exec/dg_execcheck.h \
    ui/common/dg_requestposition.h \
    ui/task/dg_takein.h \
    ui/exec/dg_exectakein.h

FORMS    += \
    ui/common/dg_loginpage.ui \
    ui/common/dg_functionoptpage.ui \
    ui/task/dg_returnpage.ui \
    ui/exec/dg_exectakeout.ui \
    ui/exec/dg_execreturn.ui \
    ui/task/dg_takeoutpage.ui \
    ui/exec/dg_execcheck.ui \
    ui/common/dg_requestposition.ui \
    ui/task/dg_takein.ui \
    ui/exec/dg_exectakein.ui

RESOURCES += \
    resource/resoure.qrc

include($$PWD/inputnew/inputnew.pri)
