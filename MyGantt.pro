QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += GANTTDEBUG

SOURCES += \
    DiagramBody.cpp \
    DiagramHat.cpp \
    DiagramHorizontalLines.cpp \
    DiagramRect.cpp \
    DiagramViewer.cpp \
    DimensionCounter.cpp \
    TestingWidget.cpp \
    main.cpp \
    GanttViewer.cpp

HEADERS += \
    DiagramBody.h \
    DiagramHat.h \
    DiagramHorizontalLines.h \
    DiagramRect.h \
    DiagramViewer.h \
    DimensionCounter.h \
    GanttViewer.h \
    TestingWidget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
