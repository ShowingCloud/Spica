#include <QTimer>

#include "light.h"

light::light(const QSerialPortInfo &serialportinfo, QObject *parent) : QObject(parent), info(serialportinfo)
{
    serialport->setPort(serialportinfo);
    serialport->setBaudRate(light::baudrate);
    serialport->setDataBits(light::databits);
    serialport->setParity(light::parity);
    serialport->setStopBits(light::stopbits);
    serialport->setFlowControl(light::flowcontrol);

    thread = new serialThread();
    thread->lightInstance = this;
    serialport->moveToThread(thread);
    thread->serialport = serialport;
    connect(thread, &serialThread::finished, thread, &QThread::deleteLater);
    connect(thread, &serialThread::finished, serialport, &QSerialPort::close);
    connect(thread, &serialThread::finished, serialport, &QSerialPort::deleteLater);
    thread->start();
}

void serialThread::run()
{
    QTimer *timer = new QTimer(serialport);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [=]() {
        if (serialport->open(QIODevice::ReadWrite)) {
            qDebug() << "Serial port opened.";
            connect(serialport, &QSerialPort::readyRead, lightInstance, &light::readData);
            connect(lightInstance, &light::writeSerial, serialport, [=](const QByteArray &data){
                serialport->write(data);
            });
        } else {
            qDebug() << "Serial port open failed" << serialport->error();
            timer->start(10000);
        }
    });
    timer->start(0);

    exec();
    timer->deleteLater();
}

light::~light()
{
    thread->quit();
    thread->wait();
    qDebug() << "Serial port closed.";
}

void light::startSearching(QObject *parent)
{
    QTimer *searchSerialTimer = new QTimer(parent);
    QObject::connect(searchSerialTimer, &QTimer::timeout, parent, [=]() {
        searchSerialTimer->start(10000);
        qInfo() << "Traversing serial ports";

        for (const auto &serialportinfo : QSerialPortInfo::availablePorts())
            [&]() {
                for (light *inlist : qAsConst(light::lightList))
                    if (inlist->has(serialportinfo))
                        return;

                light *s = new light(serialportinfo);
                light::lightList << s;
                qInfo() << "Serial List: " << light::lightList;
            }();
    });
    searchSerialTimer->start(0);
}

const QVariant light::getSerialInfo()
{
    QVector<QVariantMap> arr;
    for (const light *inlist : qAsConst(light::lightList)) {
        QVariantMap obj;
        obj["portName"] = inlist->serialport->portName();
        obj["description"] = inlist->info.description();
        obj["serialNumber"] = inlist->info.serialNumber();
        obj["isOpen"] = inlist->serialport->isOpen();
        obj["lastseen"] = (inlist->lastseen == QDateTime()) ? "Never seen" : inlist->lastseen.toString();
        arr << obj;
    }
    return QVariant::fromValue(arr);
}

void light::readData()
{
    QByteArray data = serialport->readAll();
    qDebug() << "readData" << data;
    lastseen = QDateTime::currentDateTime();
}

void light::writeData(const int channel, const int lumin)
{
    QByteArray data = QByteArray("S" + (static_cast<char>('A' - 1 + channel) + QString::number(lumin).rightJustified(4, '0')).toUtf8() + "#");
    emit writeSerial(data);
}
