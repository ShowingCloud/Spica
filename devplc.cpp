#include <iso646.h>

#include "devplc.h"

devPLC::devPLC(QObject *parent) : QObject(parent)
{
    dev = new QModbusTcpClient(this);
    dev->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    dev->setConnectionParameter(QModbusDevice::NetworkAddressParameter, addr);
    dev->setTimeout(timeout);
    dev->setNumberOfRetries(retries);

    connect(dev, &QModbusTcpClient::errorOccurred, [this](QModbusDevice::Error) {
        qDebug() << "Client dev error: " << dev->errorString();
    });
    connect(dev, &QModbusTcpClient::stateChanged, [](int state) {
        qDebug() << "Client state changed: " << state;
    });

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, [=]() {
        timer->start(1000);

        if (dev->state() != QModbusDevice::ConnectedState)
            qDebug() << "Connecting: " << dev->connectDevice();
    });
    timer->start(0);
}

devPLC::~devPLC()
{
    dev->disconnectDevice();
}

bool devPLC::readData(const int start, const quint16 len, const std::function<void(QVector<quint16>)> callback) const
{
    if (dev->state() != QModbusDevice::ConnectedState) {
        qDebug() << "Not connected";
        return false;
    }

    QModbusReply *reply = dev->sendReadRequest(
                QModbusDataUnit(QModbusDataUnit::HoldingRegisters, start, len),
                serverAddr);
    if (!reply) {
        qDebug() << "Read error: " << dev->errorString();
        return false;
    } else {
        if (not reply->isFinished()) {
            connect(reply, &QModbusReply::finished, [=]() {
                if (not reply)
                    return callback({}); // this means error occurred and a retry is required

                reply->deleteLater();
                QVector<quint16> value;

                if (reply->error() == QModbusDevice::NoError) {
                    const QModbusDataUnit unit = reply->result();
                    qInfo() << "Got data: " << unit.values();

                    if (len == 1) // read state
                        return callback(unit.values());

                    if (unit.startAddress() == start)
                        value = unit.values();
                    else if (unit.startAddress() - start == value.length())
                        value.append(unit.values());
                    else {
                        qDebug() << "Replacing";
                        for (int i = 0, total = int(unit.valueCount()); i < total; ++i) {
                            value.replace(start + i, unit.value(i));
                        }
                    }

                    if (value.length() == len)
                        return callback(value);
                    else
                        return; // not calling back for partial response and continue to wait

                } else if (reply->error() == QModbusDevice::ProtocolError) {
                    qDebug() << "Read data protocol error: " << reply->errorString() << reply->rawResult().exceptionCode();
                } else {
                    qDebug() << "Read data error: " << reply->errorString() << reply->error();
                }
                return callback({});
            });
            return true;
        } else {
            delete reply;
            return false;
        }
    }
}

bool devPLC::writeData(const int start, const QVector<quint16> value, const std::function<void(bool)> callback)
{
    if (dev->state() != QModbusDevice::ConnectedState) {
        qDebug() << "Not connected";
        return false;
    }

    QModbusReply *reply = dev->sendWriteRequest(
                QModbusDataUnit(QModbusDataUnit::HoldingRegisters, start, value),
                serverAddr);
    if (!reply) {
        qDebug() << "Write error: " << dev->errorString();
        return false;
    } else {
        if (not reply->isFinished()) {
            connect(reply, &QModbusReply::finished, [=]() {
                if (not reply)
                    return callback(false); // this means error occurred and a retry is required

                reply->deleteLater();

                if (reply->error() == QModbusDevice::ProtocolError) {
                    qDebug() << "Write data error protocol: " << reply->errorString() << reply->rawResult().exceptionCode();
                } else if (reply->error() != QModbusDevice::NoError) {
                    qDebug() << "Write data error: " << reply->errorString() << reply->error();
                } else {
                    qInfo() << "Wrote data: " << reply->result().values();
                    return callback(true);
                }

                qDebug() << "Wrote data with error: " << reply->result().values();
                return callback(false);
            });
            return true;
        } else {
            delete reply;
            return false;
        }
    }
}

#ifdef QT_DEBUG

devPLCServer::devPLCServer(QObject *parent) : QObject(parent)
{
    dev = new QModbusTcpServer(this);

    dev->setMap({
                    { QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 200 } }
                });

    dev->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    dev->setConnectionParameter(QModbusDevice::NetworkAddressParameter, addr);
    dev->setServerAddress(serverAddr);
    qDebug() << "Listening: " << dev->connectDevice();

    connect(dev, &QModbusTcpServer::dataWritten, [](QModbusDataUnit::RegisterType type, int addr, int size) {
        qDebug() << "Server data written" << type << addr << size;
    });
    connect(dev, &QModbusTcpServer::stateChanged, [](int state) {
        qDebug() << "Server state changed: " << state;
    });
    connect(dev, &QModbusTcpServer::errorOccurred, [this](QModbusDevice::Error) {
        qDebug() << "Server dev error: " << dev->errorString();
    });
}

devPLCServer::~devPLCServer()
{
    dev->disconnectDevice();
}

#endif
