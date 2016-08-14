#include <QCoreApplication>
#include <QDebug>

#include <QSerialPort>
#include <QSerialPortInfo>
#include "serial.h"

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

// Example use SerialPort
 QSerialPort serial;
 serial.setPort(info);
 if (serial.open(QIODevice::ReadWrite))
 serial.close();
 }

system("CLS");

const char cmd1[] = { 4, 7, 1, 0, 0, 0, 0 };
const char cmd2[] = { 3, 4, 1, 8 };


serialDev DMXPort("COM3", 256000);
serialDev GeneralPort("COM4", 115000);

QByteArray TXData(cmd2, sizeof(cmd2));

QByteArray TXData2(cmd1, sizeof(cmd1));

GeneralPort.TXData.enqueue(TXData);
//GeneralPort.TXData.enqueue(TXData2);

for(int i = 0; i < 50; i++){
    GeneralPort.TXData.enqueue(TXData2);
}

return a.exec();
}
