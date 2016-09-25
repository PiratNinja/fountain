#ifndef SERIAL
#define SERIAL

#include <windows.h>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <iostream>
#include <QTimer>
#include <QQueue>
#include <QList>
#include <QStringList>

struct Frame {
    QByteArray data;
    int delay;
};

class serialDev : public QObject {
    Q_OBJECT

private:
    QByteArray RX_data;
    QTimer TXTimer;
    int packetNumber;

public:
    serialDev(QString portName, int BaudRate, QSerialPort::StopBits stopBitCnt);
    void passLine(QString string, int delay);

    QQueue<Frame> TXData;
    QString PortName;

    QSerialPort port;

public slots:
    void read();
    void queueProc();

signals:
    void frameRead();
};



#endif // SERIAL

