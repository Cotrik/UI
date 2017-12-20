#-------------------------------------------------
#
# Project created by QtCreator 2015-07-01T13:29:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HexView
TEMPLATE = app


SOURCES +=\
    mainwindow.cxx \
    main.cpp \
    dataobject.cpp \
    mainwindow.cpp \
    MouseInteractorStyle.cpp \
    HexDockWidget.cpp \
    ModelBrowser.cpp \
    SheetDecompositionsDockWidget.cpp

HEADERS  += mainwindow.h \
    MouseInteractorStyle.h \
    dataobject.h \
    ui_mainwindow.h \
    ui_mainwindow.h \
    HexDockWidget.h \
    ModelBrowser.h \
    SheetDecompositionsDockWidget.h

FORMS    += \
    mainwindow.ui

INCLUDEPATH += "/usr/local/include/vtk-6.3"
LIBS += ${VTK_LIBRARIES}

RESOURCES += \
    HexView.qrc

OTHER_FILES += \
    view.qml

DISTFILES += \
    CMakeLists.txt
