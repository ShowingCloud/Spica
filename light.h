#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QThreadPool>
#include <QRunnable>
#include <iso646.h>
#include <QDebug>

class serialThread;

class light : public QObject
{
    Q_OBJECT
public:
    explicit light(const QSerialPortInfo &serialportinfo, QObject *parent = nullptr);
    ~light() override;

    static void startSearching(QObject *parent = nullptr);

    inline bool has(const QSerialPortInfo &info) const {
        return serialport->portName() == info.portName();
    }

    inline int lightOn(const int channel) {
        if (Q_LIKELY(channel > 0 and channel < 5)) {
            writeData(channel, 255);
            return 1;
        } else {
            qDebug() << "Wrong channel for light" << channel;
            return -1;
        }
    }

    inline int lightOff(const int channel) {
        if (Q_LIKELY(channel > 0 and channel < 5)) {
            writeData(channel, 0);
            return 1;
        } else {
            qDebug() << "Wrong channel for light" << channel;
            return -1;
        }
    }

    inline static void allLightsOn() {
        for (light *l : qAsConst(lightList))
            for (int i = 1; i < 5; ++i)
                l->lightOn(i);
    }

    inline static void allLightsOff() {
        for (light *l : qAsConst(lightList))
            for (int i = 1; i < 5; ++i)
                l->lightOff(i);
    }

    void lightsOff(const QVector<int> channels) {
        for (const int channel: channels)
            lightOff(channel);
    }

    inline const static enum QSerialPort::BaudRate baudrate = QSerialPort::Baud19200;
    inline const static enum QSerialPort::DataBits databits = QSerialPort::Data8;
    inline const static enum QSerialPort::Parity parity = QSerialPort::NoParity;
    inline const static enum QSerialPort::StopBits stopbits = QSerialPort::OneStop;
    inline const static enum QSerialPort::FlowControl flowcontrol = QSerialPort::NoFlowControl;

    inline static QVector<light *> lightList = QVector<light *>();

    friend class process;
    friend class frontend;

private:
    void writeData(const int channel, const int lumin);
    static const QVariant getSerialInfo();

    QDateTime lastseen;
    inline const static int timeout = 10;
    serialThread *thread;
    QSerialPort *serialport = new QSerialPort();
    const QSerialPortInfo info;

signals:
    void openSerial();
    void writeSerial(const QByteArray &data);

public slots:
    void readData();
};

class serialThread : public QThread
{
    Q_OBJECT

    void run() override;

    friend class light;

private:
    QSerialPort *serialport;
    light *lightInstance;
};

#endif // LIGHT_H
