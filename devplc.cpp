#include "devplc.h"

devPLC::devPLC(QObject *parent)
{
    Q_UNUSED(parent)

    this->dev = new QModbusTcpClient(this);
    this->dev->setConnectionParameter(QModbusDevice::NetworkPortParameter, this->port);
    this->dev->setConnectionParameter(QModbusDevice::NetworkAddressParameter, this->addr);
    this->dev->setTimeout(this->timeout);
    this->dev->setNumberOfRetries(this->retries);
    qDebug() << "Connecting: " << this->dev->connectDevice();

    connect(this->dev, &QModbusTcpClient::errorOccurred, [this](QModbusDevice::Error) {
        qDebug() << "Dev error: " << this->dev->errorString();
    });
    connect(this->dev, &QModbusClient::stateChanged,
            this, &devPLC::onModbusStateChanged);

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, [=]() {
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

void devPLC::onModbusStateChanged(int state)
{
    qDebug() << "State changed: " << state;

}

void devPLC::readData()
{
    QModbusReply *reply = this->dev->sendReadRequest(
                QModbusDataUnit(QModbusDataUnit::HoldingRegisters, this->startRead, this->lenRead),
                this->serverAddr);
    if (!reply) {
        qDebug() << "Read error: " << this->dev->errorString();
    } else {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, [=]() {
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
    QModbusReply *reply = this->dev->sendReadRequest(
                QModbusDataUnit(QModbusDataUnit::HoldingRegisters, this->addrState, 1),
                this->serverAddr);
    if (!reply) {
        qDebug() << "Read state error: " << this->dev->errorString();
    } else {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, [=]() {
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
     QModbusReply *reply = this->dev->sendWriteRequest(
                 QModbusDataUnit(QModbusDataUnit::HoldingRegisters, this->startWrite, this->writeValue),
                 this->serverAddr);
     if (!reply) {
         qDebug() << "Write error: " << this->dev->errorString();
     } else {
         if (!reply->isFinished()) {
             connect(reply, &QModbusReply::finished, this, [=]() {
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

void devPLC::writeState()
{
     QModbusReply *reply = this->dev->sendWriteRequest(
                 QModbusDataUnit(QModbusDataUnit::HoldingRegisters, this->addrState, 1),
                 this->serverAddr);
     if (!reply) {
         qDebug() << "Write error: " << this->dev->errorString();
     } else {
         if (!reply->isFinished()) {
             connect(reply, &QModbusReply::finished, this, [=]() {
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
