#ifndef PYLON_H
#define PYLON_H

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <QObject>

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
    void capture();

    enum CAM_POS { CAM_POS_1 = 1, CAM_POS_2, CAM_POS_3, CAM_POS_4, CAM_POS_5,
                   CAM_POS_6, CAM_POS_7, CAM_POS_8, CAM_POS_OTHERS };

private:
    Pylon::CInstantCamera camera;
    Pylon::CGrabResultPtr result;
    CAM_POS position;
    algorithm *algo;

    QString currentFilename;
    const quint8 *currentImage;
    int currentImageSize;
    int currentImageWidth;
    int currentImageHeight;

    inline static QList<pylon *> devList = {};
    inline static const QString savePath = "images";
};

#endif // PYLON_H
