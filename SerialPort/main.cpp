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

//serialDev DMXPort("COM3", 256000, QSerialPort::TwoStop);
//serialDev GeneralPort("COM4", 115000, QSerialPort::OneStop);
serialDev io("COM3", 115200, QSerialPort::OneStop);

uint bulbsCount = 72;
uint uartPause = 2;

QString onString, offString;

onString.push_back("1");
offString.push_back("1");

for(int i=0; i < 9; i++)
    onString.push_back(" 170");

for(int i=0; i < 10; i++)
    offString.push_back(" 0");

//GeneralPort.passLine("3 4 1 " + QString::number(bulbsCount), uartPause);

int switchCount = 1;
while(switchCount--) {
    int c = 7200;
    while(c--) {
        io.passLine(onString,  2);
    }

//    c = 50;
//    while(c--) {
//        io.passLine(offString, 10);
//    }
    }
return a.exec();
}
