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
    serialDev DMXPort("COM3", 230400, QSerialPort::TwoStop);
    while(1){
        QString str = DMXPort.port.readAll().toHex();
        std::cout << "str.toStdString()" << std::endl;
        Sleep(1);
    }
}

struct Bulb {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t strobo;
};

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

    //serialDev DMXPort("COM3", 250000, QSerialPort::TwoStop);
    serialDev GeneralPort("COM8", 230400, QSerialPort::OneStop);

    uint bulbsCount = 40;

    QString correctStr = "3 4 1 ";
    QString incorrectStr;

    for(int i = 0; i < 2000; i++) {
        incorrectStr.push_back("170 ");
    }

    GeneralPort.passLine(correctStr + QString::number(bulbsCount), 10);

    int c = 10;

    Bulb bulbs[72];

    for (int i = 0; i < 72; ++i )
        bulbs[i] = {(uint8_t) i, (uint8_t) i, (uint8_t) i, (uint8_t) 0};

    QString superComand = "10 72 0";
    for (int i = 0; i < 72; ++i ) {
        superComand.push_back(' ');
        superComand.push_back(QString::number(bulbs[i].red));
        superComand.push_back(' ');
        superComand.push_back(QString::number(bulbs[i].green));
        superComand.push_back(' ');
        superComand.push_back(QString::number(bulbs[i].blue));
        superComand.push_back(' ');
        superComand.push_back(QString::number(bulbs[i].strobo));
    }
    while(c--){
        GeneralPort.passLine(superComand, 20);
    }

    GeneralPort.passLine("9 3 1", 20);
    GeneralPort.passLine("8 3 1", 20);

    GeneralPort.queueProc();

    return a.exec();
}
