QT += core
QT -= gui
QT += serialport

TARGET = SerialPort_
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += main.cpp \
    serial.cpp

HEADERS += \
    serial.h

