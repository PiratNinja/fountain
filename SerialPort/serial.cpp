#include "serial.h"

serialDev::serialDev(QString portName, int BaudRate, QSerialPort::StopBits stopBitCnt){
    PortName = portName;
    port.setPortName(portName);
    packetNumber = 1;

    if (port.open(QIODevice::ReadWrite)) {
        port.setBaudRate(BaudRate);
        port.setDataBits(QSerialPort::Data8);
        port.setStopBits(stopBitCnt);

        connect(&port, SIGNAL(readyRead()), this, SLOT(read()));
        connect(&TXTimer, SIGNAL(timeout()), this, SLOT(queueProc()));\

        TXTimer.start();
    }
}

void serialDev::read() {
    RX_data = port.readAll();
    int i = RX_data.size();

    std::cout << PortName.toStdString() << " "
              << "PACKET NUMBER: " << packetNumber++
              << " RECIVE COUNTER: " << i << std::endl;
    QString str = QString(RX_data.toHex());
    std::cout << str.toStdString() << std::endl;

    emit frameRead();
}

void serialDev::passLine(QString string, int delay){
	QStringList list = string.split(" ");

    Frame frame;
    for(auto i = list.begin(); i != list.end(); i++) {
        frame.data.push_back((*i).toInt());
    }
    frame.delay = delay;

    TXData.enqueue(frame);
}

void serialDev::queueProc(void){

    TXTimer.stop();
    if(TXData.size()) {
        Frame temp = TXData.dequeue();
        port.write(temp.data);
        TXTimer.start(temp.delay);
    }
}
