#ifndef PYLON_H
#define PYLON_H

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <QObject>
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

    enum CAM_POS { CAM_POS_1 = 1, CAM_POS_2, CAM_POS_3, CAM_POS_4, CAM_POS_5,
                   CAM_POS_6, CAM_POS_7, CAM_POS_8, CAM_POS_OTHERS };
    inline static const QVector<CAM_POS> camposList = { CAM_POS_1, CAM_POS_2, CAM_POS_3, CAM_POS_4,
                                                        CAM_POS_5, CAM_POS_6, CAM_POS_7, CAM_POS_8 };

    friend class process;

signals:
    void gotAlgo(const int imgId, const int algoId, const QVector<int> result) const;

private:
    Pylon::CInstantCamera camera;
    Pylon::CGrabResultPtr result;
    CAM_POS position;
    devPLC::CAM_POS station;
    algorithm *algo;

    QString currentFilename;
    const quint8 *currentImage;
    int currentImageSize;
    int currentImageWidth;
    int currentImageHeight;
    int imgId;

    inline static QVector<pylon *> devList = {};
    inline static QHash<CAM_POS, pylon *> posDevList = {};
    inline static const QString savePath = "images";

    inline static const QHash<CAM_POS, devPLC::CAM_POS> positionStation = {
        { CAM_POS_1, devPLC::CAM_POS_B },
        { CAM_POS_2, devPLC::CAM_POS_C },
        { CAM_POS_3, devPLC::CAM_POS_B },
        { CAM_POS_4, devPLC::CAM_POS_C },
        { CAM_POS_5, devPLC::CAM_POS_B },
        { CAM_POS_6, devPLC::CAM_POS_C },
        { CAM_POS_7, devPLC::CAM_POS_B },
        { CAM_POS_8, devPLC::CAM_POS_D }};
    inline static const QHash<int, CAM_POS> idPosition = {
        { 0, CAM_POS_1 },
        { 1, CAM_POS_2 },
        { 2, CAM_POS_3 },
        { 3, CAM_POS_4 },
        { 4, CAM_POS_5 },
        { 5, CAM_POS_6 },
        { 6, CAM_POS_7 },
        { 7, CAM_POS_8 }};
};

#endif // PYLON_H
