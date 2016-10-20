TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += `sdl-config --libs` -lSDLmain -lSDL -lSDL_image -lSDL_ttf
INCLUDEPATH += -I/usr/include/SDL/

SOURCES += main.cpp

include(deployment.pri)
qtcAddDeployment()

