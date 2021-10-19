#ifndef PYLON_H
#define PYLON_H

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <QObject>

class database;

class pylon : public QObject
{
    Q_OBJECT

public:
    explicit pylon(const Pylon::CDeviceInfo dev, QObject *parent = nullptr);
    ~pylon() override;

    friend database &operator<< (database &db, const pylon &py);

    static void initialize();
    void capture();

private:
    Pylon::CInstantCamera camera;
    Pylon::CGrabResultPtr result;
    QString currentFilename;

    inline static QList<pylon *> devList = {};
    inline static const QString savePath = "images";
};

#endif // PYLON_H
