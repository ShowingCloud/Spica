#ifndef DEVPLC_H
#define DEVPLC_H

#include <QObject>
#include <QTimer>
#include <QModbusTcpClient>
#include <QDebug>

class devPLC : public QObject
{
    Q_OBJECT

public:
    explicit devPLC(QObject *parent = nullptr);
    ~devPLC() override;

public slots:
    void onModbusStateChanged(int state);
    void readData();
    void readState();
    void writeData();
    void writeState();


private:
    const QString addr = "192.168.2.1";
    const int port = 502;
    const int timeout = 1000;
    const int retries = 3;
    const int serverAddr = 255;
    const int startRead = 1000;
    const quint16 lenRead = 140;
    const int startWrite = 1140;
    const quint16 lenWrite = 18;
    const int addrState = 1200;

    QModbusTcpClient *dev = nullptr;
    QTimer *timer = nullptr;
    QVector<quint16> readValue;
    QVector<quint16> writeValue = QVector<quint16>(lenWrite, 1);
    QVector<quint16> stateValue;
};

#endif // DEVPLC_H
