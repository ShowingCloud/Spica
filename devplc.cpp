#include "devplc.h"

devPLC::devPLC(QObject *parent) : QObject(parent)
{
    Q_UNUSED(parent)

    this->dev = new QModbusTcpClient(this);
    this->dev->setConnectionParameter(QModbusDevice::NetworkPortParameter, this->port);
    this->dev->setConnectionParameter(QModbusDevice::NetworkAddressParameter, this->addr);
    this->dev->setTimeout(this->timeout);
    this->dev->setNumberOfRetries(this->retries);
    qDebug() << "Connecting: " << this->dev->connectDevice();

    connect(this->dev, &QModbusTcpClient::errorOccurred, [this](QModbusDevice::Error) {
        qDebug() << "Client dev error: " << this->dev->errorString();
    });
    connect(this->dev, &QModbusTcpClient::stateChanged, [](int state) {
        qDebug() << "Client state changed: " << state;
    });

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, [=]() {
#ifdef QT_DEBUG
        this->writeReadData();
        this->writeState();
#endif
        this->readState();
        this->readData();
        timer->start(1000);
    });
    timer->start(0);
}

devPLC::~devPLC()
{
    this->dev->disconnectDevice();
}

void devPLC::readData()
{
    if (this->dev->state() != QModbusDevice::ConnectedState) {
        qDebug() << "Not connected";
        return;
    }

    QModbusReply *reply = this->dev->sendReadRequest(
                QModbusDataUnit(QModbusDataUnit::HoldingRegisters, this->startRead, this->lenRead),
                this->serverAddr);
    if (!reply) {
        qDebug() << "Read error: " << this->dev->errorString();
    } else {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, [=]() {
                if (!reply)
                    return;

                if (reply->error() == QModbusDevice::NoError) {
                    const QModbusDataUnit unit = reply->result();
                    qDebug() << "Got data: " << unit.values();

                    if (unit.startAddress() == this->startRead)
                        this->readValue = unit.values();
                    else if (unit.startAddress() - this->startRead == this->readValue.length())
                        this->readValue.append(unit.values());
                    else {
                        qDebug() << "Replacing";
                        for (int i = 0, total = int(unit.valueCount()); i < total; ++i) {
                            this->readValue.replace(this->startRead + i, unit.value(i));
                        }
                    }
                } else if (reply->error() == QModbusDevice::ProtocolError) {
                    qDebug() << "Read response error: " << reply->errorString() << reply->rawResult().exceptionCode();
                } else {
                    qDebug() << "Read response error: " << reply->errorString() << reply->error();
                }

                reply->deleteLater();
            });
        else
            delete reply;
    }
}

void devPLC::readState()
{
    if (this->dev->state() != QModbusDevice::ConnectedState) {
        qDebug() << "Not connected";
        return;
    }

    QModbusReply *reply = this->dev->sendReadRequest(
                QModbusDataUnit(QModbusDataUnit::HoldingRegisters, this->addrState, 1),
                this->serverAddr);
    if (!reply) {
        qDebug() << "Read state error: " << this->dev->errorString();
    } else {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, [=]() {
                if (!reply)
                    return;

                if (reply->error() == QModbusDevice::NoError) {
                    const int state = reply->result().value(0);
                    qDebug() << "Got state: " << state;

                    if (state == 1) {
                        this->readData();
                        this->writeData();
                        this->writeState();
                    }
                } else if (reply->error() == QModbusDevice::ProtocolError) {
                    qDebug() << "Got wrong state: " << reply->errorString() << reply->rawResult().exceptionCode();
                } else {
                    qDebug() << "Got wrong state: " << reply->errorString() << reply->error();
                }

                reply->deleteLater();
            });
        else
            delete reply;
    }
}

void devPLC::writeData()
{
    if (this->dev->state() != QModbusDevice::ConnectedState) {
        qDebug() << "Not connected";
        return;
    }

    QModbusReply *reply = this->dev->sendWriteRequest(
                QModbusDataUnit(QModbusDataUnit::HoldingRegisters, this->startWrite, this->writeValue),
                this->serverAddr);
    if (!reply) {
        qDebug() << "Write error: " << this->dev->errorString();
    } else {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, [=]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    qDebug() << "Write response error: " << reply->errorString() << reply->rawResult().exceptionCode();
                } else if (reply->error() != QModbusDevice::NoError) {
                    qDebug() << "Write response error: " << reply->errorString() << reply->error();
                }
                qDebug() << "Wrote: " << reply->result().values();

                reply->deleteLater();
            });
        } else {
            reply->deleteLater();
        }
    }
}

#ifdef QT_DEBUG
void devPLC::writeReadData()
{
    if (this->dev->state() != QModbusDevice::ConnectedState)
        return;

    QModbusReply *reply = this->dev->sendWriteRequest(
                QModbusDataUnit(QModbusDataUnit::HoldingRegisters, this->startRead, this->readValueAllOne),
                this->serverAddr);
    reply->deleteLater();
}
#endif

void devPLC::writeState()
{
    if (this->dev->state() != QModbusDevice::ConnectedState) {
        qDebug() << "Not connected";
        return;
    }

    QModbusReply *reply = this->dev->sendWriteRequest(
                QModbusDataUnit(QModbusDataUnit::HoldingRegisters, this->addrState, QVector<quint16>(1, 2)),
                this->serverAddr);
    if (!reply) {
        qDebug() << "Write error: " << this->dev->errorString();
    } else {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, [=]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    qDebug() << "Write state error: " << reply->errorString() << reply->rawResult().exceptionCode();
                } else if (reply->error() != QModbusDevice::NoError) {
                    qDebug() << "Write state error: " << reply->errorString() << reply->error();
                }
                qDebug() << "Wrote: " << reply->result().values();

                reply->deleteLater();
            });
        } else {
            reply->deleteLater();
        }
    }
}

#ifdef QT_DEBUG

devPLCServer::devPLCServer(QObject *parent) : QObject(parent)
{
    this->dev = new QModbusTcpServer(this);

    this->dev->setMap({
                          { QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 1300 } }
                      });

    this->dev->setConnectionParameter(QModbusDevice::NetworkPortParameter, this->port);
    this->dev->setConnectionParameter(QModbusDevice::NetworkAddressParameter, this->addr);
    this->dev->setServerAddress(this->serverAddr);
    qDebug() << "Listening: " << this->dev->connectDevice();

    connect(this->dev, &QModbusTcpServer::dataWritten, [](QModbusDataUnit::RegisterType type, int addr, int size) {
        Q_UNUSED(type)
        Q_UNUSED(addr)
        Q_UNUSED(size)
    });
    connect(this->dev, &QModbusTcpServer::stateChanged, [](int state) {
        qDebug() << "Server state changed: " << state;
    });
    connect(this->dev, &QModbusTcpServer::errorOccurred, [this](QModbusDevice::Error) {
        qDebug() << "Server dev error: " << this->dev->errorString();
    });
}

devPLCServer::~devPLCServer()
{
    this->dev->disconnectDevice();
}

#endif
