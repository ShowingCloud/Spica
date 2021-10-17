#include <QTimer>
#include <QImage>
#include <QtDebug>
#include <QDir>
#include <QDateTime>
#include <QUuid>

#include "pylon.h"

pylon::pylon(const Pylon::CDeviceInfo dev, QObject *parent)
    : QObject(parent), camera(Pylon::CTlFactory::GetInstance().CreateDevice(dev))
{
    qDebug() << "Using device: " << camera.GetDeviceInfo().GetModelName();
    camera.MaxNumBuffer = 5;

    QTimer *captureTimer = new QTimer(this);
    QObject::connect(captureTimer, &QTimer::timeout, [&]() {
        capture();
    });
    captureTimer->start(1000);
}

pylon::~pylon()
{
    Pylon::PylonTerminate();
}

void pylon::initialize()
{
    Pylon::PylonInitialize();
    Pylon::DeviceInfoList pylonList;
    if (Pylon::CTlFactory::GetInstance().EnumerateDevices(pylonList) != 0)
        for (const Pylon::CDeviceInfo &dev : qAsConst(pylonList)) {
            pylon *p = new pylon(dev);
            devList << p;
        }

    QDir::current().mkdir(savePath);
}

void pylon::capture()
{
    camera.GrabOne(5000, result);

    if (result->GrabSucceeded())
        QImage img(static_cast<const quint8 *>(result->GetBuffer()),
                   static_cast<int>(result->GetWidth()),
                   static_cast<int>(result->GetHeight()),
                   QImage::Format_RGB888);
    else
        qDebug() << "Error: " << Qt::hex << result->GetErrorCode() << Qt::dec << " " <<
                    result->GetErrorDescription();

    QString filepath = savePath + "/" + QDateTime::currentDateTime().toString(Qt::ISODate) + "-"
                                        + QUuid::createUuid().toString(QUuid::Id128) + ".png";
    Pylon::CImagePersistence::Save(Pylon::EImageFileFormat::ImageFileFormat_Png,
                                   filepath.toStdString().c_str(), result);
}
