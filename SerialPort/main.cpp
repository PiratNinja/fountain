     #include <QCoreApplication>
#include <QDebug>

#include <QSerialPort>
#include <QSerialPortInfo>
#include "serial.h"
#include <iostream>
#include "windows.h"

QT_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Example use SerialPortInfo
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        qDebug() << "Standard BaudRates: " << info.standardBaudRates();
    }
    //system("CLS");

    serialDev DMXPort("COM3", 256000, QSerialPort::TwoStop);
    serialDev GeneralPort("COM5", 115200, QSerialPort::OneStop);

    uint bulbsCount = 72;

    GeneralPort.passLine("3 4 1 " + QString::number(bulbsCount), 2);

    int c = 1;
    while(c--){
        for(int i = 0; i < bulbsCount; i++) {
            GeneralPort.passLine("4 7 1 " + QString::number(i) + " 0 0 0", 2);
        }
        //update
        GeneralPort.passLine("8 3 1", 2);

        for(int i = 0; i < bulbsCount; i++) {
            GeneralPort.passLine("4 7 1 " + QString::number(i) + " 255 255 255", 2);
        }
        //update
        GeneralPort.passLine("8 3 1", 2);
    }
    c = 10;
    while(c--)
        GeneralPort.passLine("9 3 1", 2);

    return a.exec();
}
