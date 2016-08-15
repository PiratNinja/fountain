#include "serial.h"

serialDev::serialDev(QString portName, int BaudRate){
    PortName = portName;
    port.setPortName(portName);
    packetNumber = 1;

    if (port.open(QIODevice::ReadWrite)) {
        port.setBaudRate(BaudRate);
        port.setDataBits(QSerialPort::Data8);
        port.setStopBits(QSerialPort::OneStop);

        connect(&port, SIGNAL(readyRead()), this, SLOT(read()));
        connect(&TXTimer, SIGNAL(timeout()), this, SLOT(queueProc()));

        TXTimer.start(10);
    }
	//debug
	connect(&TXTimer, SIGNAL(timeout()), this, SLOT(queueProc()));
	TXTimer.start(1000);
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

void serialDev::passLine(QString string){
	QStringList list = string.split(" ");

	QByteArray ba;
    for(auto i = list.begin(); i != list.end(); i++) {
		ba.push_back((*i).toInt());
    }
	TXData.enqueue(ba);
}

void serialDev::queueProc(void){

//    if(TXData.size()) {
//        port.write(TXData.dequeue());
//    }

	//debug

	std::cout << ;
	if(packetNumber++ %2) std::cout << "\rsome string";
	else std::cout << "\rsome longer string";
}
