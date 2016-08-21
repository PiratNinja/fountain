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
serialDev GeneralPort("COM4", 115000, QSerialPort::OneStop);

uint bulbsCount = 72;

GeneralPort.passLine("3 4 1 " + QString::number(bulbsCount), 2);

while(c--){
    for(int i = 0; i < bulbsCount; i++) {
        GeneralPort.passLine("4 7 1 " + QString::number(i) + " 0 0 0", 1);
    }
    //update
    GeneralPort.passLine("8 3 1", 20);

    for(int i = 0; i < bulbsCount; i++) {
        GeneralPort.passLine("4 7 1 " + QString::number(i) + " 255 255 255", 1);
    }
    //update
    GeneralPort.passLine("8 3 1", 20);
}

return a.exec();
}
