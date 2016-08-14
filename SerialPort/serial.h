#ifndef SERIAL
#define SERIAL

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <iostream>
#include <QTimer>
#include <QQueue>
#include <QList>

class serialDev : public QObject {
    Q_OBJECT

private:
    QSerialPort port;
    QByteArray RX_data;
    QTimer TXTimer;
    int packetNumber;

public:
    serialDev(QString portName, int BaudRate);
    void passLine(QString string);

    QQueue<QByteArray> TXData;
    QString PortName;

public slots:
    void read();
    void queueProc();

signals:
    void frameRead();
};

#endif // SERIAL

