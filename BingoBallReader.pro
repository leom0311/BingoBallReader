QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Carton.cpp \
    camerathread.cpp \
    engine.cpp \
    licencia.cpp \
    main.cpp \
    admincontrol.cpp \
    EngineThread.cpp \
    ordenaciones.cpp \
    videothread.cpp

HEADERS += \
    Carton.h \
    admincontrol.h \
       EngineThread.h \
    camerathread.h \
    engine.h \
    licencia.h \
    ordenaciones.h \
    videothread.h

FORMS += \
    admincontrol.ui

TRANSLATIONS += \
    BingoBallReader_es_ES.ts
CONFIG += lrelease
CONFIG += embed_translations

INCLUDEPATH += $$PWD/include
LIBS += $$PWD/lib/libopencv_world470.dll

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Recursos.qrc
