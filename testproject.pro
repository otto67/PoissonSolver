QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17


DISTFILES += \
    plotter.py

MY_PWD = $$PWD
MY_OUT_PWD = $$OUT_PWD

win32 {
MY_PWD ~= s,/,\\,g
MY_OUT_PWD ~= s,/,\\,g
}

QMAKE_POST_LINK +=  $$QMAKE_COPY $$quote($$MY_PWD\\plotter.py $$MY_OUT_PWD)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    FDPoisson.cpp \
    Simulator.cpp \
    gauss.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    FDPoisson.h \
    Simulator.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

