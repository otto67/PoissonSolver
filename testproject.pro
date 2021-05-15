QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

DISTFILES += \
    plotter.py \
    livigno.jpg \
    mountain.jpg

MY_PWD = $$PWD
MY_OUT_PWD = $$OUT_PWD
MY_FILES = $$MY_PWD/plotter.py $$MY_PWD/livigno.jpg $$MY_PWD/mountain.jpg

win32 {
MY_PWD ~= s,/,\\,g
MY_OUT_PWD ~= s,/,\\,g
MY_FILES ~= s,/,\\,g
}

for(a, MY_FILES){
                QMAKE_POST_LINK +=  $$QMAKE_COPY $$quote($${a} $$MY_OUT_PWD $$escape_expand(\\n\\t))
}

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

