QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    matrix_impl.cpp \
    northwest_corner_method.cpp \
    potentials_method.cpp \
    secondwindow.cpp \
    vector_impl.cpp

HEADERS += \
    mainwindow.h \
    matrix_impl.h \
    northwest_corner_method.h \
    potentials_method.h \
    secondwindow.h \
    vector_impl.h

FORMS += \
    mainwindow.ui \
    secondwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
