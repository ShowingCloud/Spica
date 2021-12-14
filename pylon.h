#ifndef PYLON_H
#define PYLON_H

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <QObject>
#include <QJsonDocument>
#include <iso646.h>

#include "devplc.h"

class database;
class algorithm;

class pylon : public QObject
{
    Q_OBJECT

public:
    explicit pylon(const Pylon::CDeviceInfo dev, QObject *parent = nullptr);
    ~pylon() override;

    friend database &operator<< (database &db, const pylon &py);
    friend algorithm &operator<< (algorithm &algo, const pylon &cam);

    static void initialize(QObject *parent);
    static void destroy();
    int capture();
    int lightOn();
    int lightOff();

    enum CAM_POS { CAM_POS_1 = 1, CAM_POS_2, CAM_POS_3, CAM_POS_4, CAM_POS_5,
                   CAM_POS_6, CAM_POS_7, CAM_POS_8, CAM_POS_OTHERS };
    inline static const QVector<CAM_POS> camposList = { CAM_POS_1, CAM_POS_2, CAM_POS_3, CAM_POS_4,
                                                        CAM_POS_5, CAM_POS_6, CAM_POS_7, CAM_POS_8 };

    friend class process;
    friend class frontend;

signals:
    void gotAlgo(const int imgId, const int algoId, const QVector<int> result) const;

private:
    static const QVariant getCameraInfo();

    Pylon::CInstantCamera camera;
    Pylon::CGrabResultPtr result;
    CAM_POS position;
    devPLC::CAM_POS station;
    algorithm *algo;
    bool light;

    QString currentFilename;
    const quint8 *currentImage;
    int currentImageSize;
    int currentImageWidth;
    int currentImageHeight;
    int imgId;

    inline static QVector<pylon *> devList = {};
    inline static QHash<CAM_POS, pylon *> posDevList = {};
    inline static const QString savePath = "images";

    inline static QHash<CAM_POS, devPLC::CAM_POS> positionStation = {};
    inline static const QJsonDocument defaultPositionStation = QJsonDocument::fromJson("{\
        \"positionStation\": [\
            { \"position\": 1, \"station\": 0 },\
            { \"position\": 2, \"station\": 1 },\
            { \"position\": 3, \"station\": 2 },\
            { \"position\": 4, \"station\": 1 },\
            { \"position\": 5, \"station\": 0 },\
            { \"position\": 6, \"station\": 1 },\
            { \"position\": 7, \"station\": 2 },\
            { \"position\": 8, \"station\": 0 }\
        ]}");
    inline static QHash<int, CAM_POS> idPosition = {};
    inline static const QJsonDocument defaultIdPosition = QJsonDocument::fromJson("{\
        \"idPosition\": [\
            { \"id\": 23932524, \"position\": 1 },\
            { \"id\": 23932524, \"position\": 2 },\
            { \"id\": 23932524, \"position\": 3 },\
            { \"id\": 23932524, \"position\": 4 },\
            { \"id\": 23932524, \"position\": 5 },\
            { \"id\": 23932524, \"position\": 6 },\
            { \"id\": 23932524, \"position\": 7 },\
            { \"id\": 23932524, \"position\": 8 }\
        ]}");
};

class pylonImageEventHandler : public Pylon::CImageEventHandler
{
public:
    virtual void onImageGrabbed(Pylon::CInstantCamera &camera, const Pylon::CGrabResultPtr &result) {
        qDebug() << "onImageGrabbed";
    }
};

#endif // PYLON_H
