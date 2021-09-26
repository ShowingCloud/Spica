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
    void readData();
    void readState();
    void writeData();
    void writeState();
#ifdef QT_DEBUG
    void writeReadData();
#endif

private:
#ifdef QT_DEBUG
    const QString addr = "127.0.0.1";
    const int port = 10502;
    //const QString addr = "192.168.107.188";
    //const int port = 502;
#else
    const QString addr = "192.168.2.1";
    const int port = 502;
#endif
    const int timeout = 1000;
    const int retries = 3;
    const int serverAddr = 255;
    const int startRead = 0;
    const quint16 lenRead = 68;
    const int startWrite = 70;
    const quint16 lenWrite = 9;
    const int addrState = 100;

    QModbusTcpClient *dev = nullptr;
    QTimer *timer = nullptr;
    QVector<quint16> readValue;
    QVector<quint16> writeValue = QVector<quint16>(lenWrite, 1);
    QVector<quint16> stateValue;
#ifdef QT_DEBUG
    const QVector<quint16> readValueAllOne = QVector<quint16>(lenRead, 1);
#endif
};


#ifdef QT_DEBUG
#include <QModbusTcpServer>

class devPLCServer : public QObject
{
    Q_OBJECT

public:
    explicit devPLCServer(QObject *parent = nullptr);
    ~devPLCServer() override;

private:
    QModbusTcpServer *dev = nullptr;
    const QString addr = "0.0.0.0";
    const int port = 10502;
    const int serverAddr = 255;
};

#else
class devPLCServer : public QObject
{
    Q_OBJECT

public:
    explicit devPLCServer(QObject *parent = nullptr) : QObject(parent) {};
};
#endif

#endif // DEVPLC_H
