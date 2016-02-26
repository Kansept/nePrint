#-------------------------------------------------
#
# Project created by QtCreator 2015-06-29T09:19:08
#
#-------------------------------------------------

QT += core gui
QT += widgets axcontainer
QT += sql widgets
QT += printsupport
QT += svg xml
QT += concurrent widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nePrint
TEMPLATE = app

INCLUDEPATH += D:/Qt/5.5/Src/qtbase/src/3rdparty/sqlite
SOURCES += D:/Qt/5.5/Src/qtbase/src/3rdparty/sqlite/sqlite3.c \
    dialogsetting.cpp \
    setting.cpp \
    textitem.cpp \
    dialogrecepient.cpp \
    recepient.cpp \
    dbrecepient.cpp \
    dialogupdate.cpp

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    MyWidget/tableviewsp.h \
    dialogsetting.h \
    setting.h \
    textitem.h \
    dialogrecepient.h \
    recepient.h \
    dbrecepient.h \
    modelrecepient.h \
    dialogupdate.h

FORMS += mainwindow.ui \
      dialogsetting.ui \
    dialogrecepient.ui \
    dialogupdate.ui

RESOURCES += \
    res.qrc

RC_FILE += app.rc
