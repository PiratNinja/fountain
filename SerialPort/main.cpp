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

onString.push_back("2");
offString.push_back("2");

for(int i=0; i<1; i++)
    onString.push_back(" 100 0");

for(int i=0; i<20; i++)
    offString.push_back(" 0 100");



//GeneralPort.passLine("3 4 1 " + QString::number(bulbsCount), uartPause);



int switchCount = 2;
while(switchCount--) {
    int c = 20;
    while(c--){
    //    for(int i = 0; i < bulbsCount; i++) {
    //       GeneralPort.passLine("4 7 1 " + QString::number(i) + " 0 0 0", uartPause);
    //       //GeneralPort.passLine("8 3 1", 20);
    //    }
    //    //update
    //    GeneralPort.passLine("8 3 1", 20);

    //    for(int i = 0; i < bulbsCount; i++) {
    //        GeneralPort.passLine("4 7 1 " + QString::number(i) + " 255 255 255", uartPause);
    //    }
    //    //update
    //    GeneralPort.passLine("8 3 1", 20);

        io.passLine(onString, 10);
    }

    c = 20;
    while(c--) {
        io.passLine(offString, 10);

    }
}
return a.exec();
}
