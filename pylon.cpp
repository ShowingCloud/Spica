#include <QTimer>
#include <QImage>
#include <QtDebug>
#include <QDir>
#include <QDateTime>
#include <QUuid>
#include <iso646.h>

#include "pylon.h"
#include "database.h"
#include "algorithm.h"

pylon::pylon(const Pylon::CDeviceInfo dev, QObject *parent)
    : QObject(parent), camera(Pylon::CTlFactory::GetInstance().CreateDevice(dev))
{
    qDebug() << "Using device: " << camera.GetDeviceInfo().GetModelName();
    camera.MaxNumBuffer = 100;

    position = CAM_POS_1;
    algo = new algorithm(position);

    QTimer *captureTimer = new QTimer(this);
    QObject::connect(captureTimer, &QTimer::timeout, [&]() {
        capture();
    });
    captureTimer->start(1000);
}

pylon::~pylon()
{
    camera.DestroyDevice();
    qDebug() << "Destroyed" << this;
    delete algo;
}

void pylon::initialize(QObject *parent)
{
    QDir::current().mkdir(savePath);

    Pylon::PylonInitialize();
    Pylon::DeviceInfoList pylonList;
    if (Pylon::CTlFactory::GetInstance().EnumerateDevices(pylonList) != 0)
        for (const Pylon::CDeviceInfo &dev : qAsConst(pylonList)) {
            pylon *p = new pylon(dev, parent);
            devList << p;
        }
}

void pylon::destroy()
{
    for (pylon *p : qAsConst(devList))
        delete p;
    Pylon::PylonTerminate();
}

void pylon::capture()
{
    camera.GrabOne(5000, result);

    if (result->GrabSucceeded()) {
        currentImage = static_cast<const quint8 *>(result->GetBuffer());
        currentImageSize = static_cast<int>(result->GetBufferSize());
        currentImageWidth = static_cast<int>(result->GetWidth());
        currentImageHeight = static_cast<int>(result->GetHeight());
        qDebug() << currentImageSize << currentImageWidth << currentImageHeight;
        *algo << *this;
        /*
        QImage img(static_cast<const quint8 *>(result->GetBuffer()),
                   static_cast<int>(result->GetWidth()),
                   static_cast<int>(result->GetHeight()),
                   QImage::Format_RGB888);
        */

        currentFilename = savePath + "/" + QDateTime::currentDateTime().toString(Qt::ISODate) + "-"
                                        + QUuid::createUuid().toString(QUuid::Id128) + ".png";
        Pylon::CImagePersistence::Save(Pylon::EImageFileFormat::ImageFileFormat_Png,
                                       currentFilename.replace(":", "-").toStdString().c_str(), result);
        *globalDB << *this;

    } else
        qDebug() << "Error: " << QString::number(result->GetErrorCode(), 16) << " " <<
                    result->GetErrorDescription();
}

database &operator<< (database &db, const pylon &py)
{
    db.dbModel->setTable(db.DB_TABLES[db.DB_TBL_IMG]);
    QSqlRecord r = db.dbModel->record();
    r.setValue("CamID", 0);
    r.setValue("StepID", py.position);
    r.setValue("Time", QDateTime::currentDateTime());
    r.setValue("Filename", py.currentFilename);
    if (!db.dbModel->insertRecord(-1, r))
        qDebug() << db.dbModel->lastError();

    return db;
}
