#############################################################################
##
## Makefile to integrate 3 key pieces of a typical system
##
##   1. Darknet/Yolo
##   2. Opencv3
##   3. Qt5
## Note: Darknet  built as a shared library
##
###############################################################################

QT += core
QT -= gui

CONFIG += c++11

TARGET = countingCar
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += /home/nesli/DarknetV3/darknet/include
LIBS += -L//home/nesli/DarknetV3/darknet -ldarknet
LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopenalpr
LIBS += -L/usr/local/cuda/lib64 -lcuda -lcudart -lcublas -lcurand -lcudnn

HEADERS += \
    carcounter.h \
    yoloclass.h
TEMPLATE = app
SOURCES += main.cpp \
    carcounter.cpp \
    yoloclass.cpp


