#ifndef PYLON_H
#define PYLON_H

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <QObject>

class pylon : public QObject
{
    Q_OBJECT

public:
    explicit pylon(const Pylon::CDeviceInfo dev, QObject *parent = nullptr);
    ~pylon() override;

    static void initialize();
    void capture();

private:
    Pylon::CInstantCamera camera;
    Pylon::CGrabResultPtr result;

    inline static QList<pylon *> devList = {};
    inline static const QString savePath = "images";
};

#endif // PYLON_H
