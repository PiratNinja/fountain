#include <QCoreApplication>
#include <QDebug>

#include <QSerialPort>
#include <QSerialPortInfo>
#include "serial.h"
#include <iostream>
#include "windows.h"
#include <thread>

QT_USE_NAMESPACE

void SPRread() {
    serialDev DMXPort("COM3", 250000, QSerialPort::TwoStop);
    while(1){
        QString str = DMXPort.port.readAll().toHex();
        std::cout << "str.toStdString()" << std::endl;
        Sleep(1);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    std::thread tSP(SPRread);
//    tSP.join();

    // Example use SerialPortInfo
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        qDebug() << "Standard BaudRates: " << info.standardBaudRates();
    }
    //system("CLS");

    serialDev DMXPort("COM3", 250000, QSerialPort::TwoStop);
    serialDev GeneralPort("COM5", 115200, QSerialPort::OneStop);

    uint bulbsCount = 10;

    QString correctStr = "3 4 1 ";
    QString incorrectStr;

    for(int i = 0; i < 2000; i++) {
        incorrectStr.push_back("170 ");
    }

    GeneralPort.passLine(correctStr + QString::number(bulbsCount), 10);

    int c = 30;
    while(c--){
        for(int i = 0; i < bulbsCount; i++) {
            GeneralPort.passLine("4 7 1 " + QString::number(i) + " 0 0 0", 10);
        }
        //update
        GeneralPort.passLine("8 3 1", 20);

        for(int i = 0; i < bulbsCount; i++) {
            GeneralPort.passLine("4 7 1 " + QString::number(i) + " 255 255 255", 10);
        }
        //update
        GeneralPort.passLine("8 3 1", 20);
    }

    GeneralPort.passLine("9 3 1", 20);

    GeneralPort.queueProc();

    return a.exec();
}
