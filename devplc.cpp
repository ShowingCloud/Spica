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
        qDebug() << "Error: " << this->dev->errorString();
    });
    connect(this->dev, &QModbusClient::stateChanged,
            this, &devPLC::onModbusStateChanged);
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
                0/*serverAddr*/);
    if (!reply->isFinished())
        connect(reply, &QModbusReply::finished, this, &devPLC::onReadReady);
    else
        delete reply;
}

void devPLC::onReadReady()
{
    QModbusReply *reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();

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
}
