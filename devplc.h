#ifndef DEVPLC_H
#define DEVPLC_H

#include <QObject>
#include <QTimer>
#include <QModbusTcpClient>
#include <QDebug>

class pylon;

class devPLC : public QObject
{
    Q_OBJECT

public:
    explicit devPLC(QObject *parent = nullptr);
    ~devPLC() override;

    enum CAM_POS { CAM_POS_B, CAM_POS_C, CAM_POS_D, CAM_POS_OTHERS };
    inline static const QVector<CAM_POS> camposList = { CAM_POS_B, CAM_POS_C, CAM_POS_D };

    inline static void addDeviceList (CAM_POS station, pylon *device) {
        devList[station] << device;
    }

    friend class process;

private:
    bool readData(const int start, const quint16 len, const std::function<void(QVector<quint16>)> callback) const;
    bool writeData(const int start, const QVector<quint16> value, const std::function<void(bool)> callback);
#ifdef QT_DEBUG
    void writeReadData();
#endif

#ifdef QT_DEBUG
    const QString addr = "127.0.0.1";
    const int port = 10502;
#else
    const QString addr = "192.168.2.3";
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

    inline static const QHash<CAM_POS, int> camReadAddr = {
        { CAM_POS_B, 66 },
        { CAM_POS_C, 67 },
        { CAM_POS_D, 68 }};
    inline static const quint16 camReadAddrLen = 3;
    inline static const QHash<CAM_POS, QVector<int>> camWriteAddr = {
        { CAM_POS_B, { 70, 73, 76 } },
        { CAM_POS_C, { 71, 74, 77 } },
        { CAM_POS_D, { 72, 75, 78 } }};
    inline static const QHash<CAM_POS, QVector<int>> camProdAddr = {
        { CAM_POS_B, { 2, 12, 22 } },
        { CAM_POS_C, { 4, 14, 24 } },
        { CAM_POS_D, { 6, 16, 26 } }};
    inline static QHash<CAM_POS, QVector<pylon *>> devList = {
        { CAM_POS_B, {} },
        { CAM_POS_C, {} },
        { CAM_POS_D, {} }};
    inline static const QVector<int> pneuAddr = { 34, 39, 44 };
    inline static const quint16 pneuAddrLen = 11;
    inline static const QVector<int> pneuProdAddr = { 8, 18, 28 };
    inline static const quint16 prodAddrLen = 22;

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
