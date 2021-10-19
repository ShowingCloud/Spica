#include <QTimer>
#include <QImage>
#include <QtDebug>
#include <QDir>
#include <QDateTime>
#include <QUuid>

#include "pylon.h"
#include "database.h"

pylon::pylon(const Pylon::CDeviceInfo dev, QObject *parent)
    : QObject(parent), camera(Pylon::CTlFactory::GetInstance().CreateDevice(dev))
{
    qDebug() << "Using device: " << camera.GetDeviceInfo().GetModelName();
    camera.MaxNumBuffer = 1;

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
}

void pylon::initialize(const QObject *parent)
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

void pylon::destroy()
{
    for (const pylon *p : qAsConst(devList))
        delete p;
    Pylon::PylonTerminate();
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
        qDebug() << "Error: " << QString::number(result->GetErrorCode(), 16) << " " <<
                    result->GetErrorDescription();

    currentFilename = savePath + "/" + QDateTime::currentDateTime().toString(Qt::ISODate) + "-"
                                        + QUuid::createUuid().toString(QUuid::Id128) + ".png";
    Pylon::CImagePersistence::Save(Pylon::EImageFileFormat::ImageFileFormat_Png,
                                   currentFilename.replace(":", "-").toStdString().c_str(), result);
    *globalDB << *this;
}

database &operator<< (database &db, const pylon &py)
{
    db.dbModel->setTable(db.DB_TABLES[db.DB_TBL_IMG]);
    QSqlRecord r = db.dbModel->record();
    r.setValue("CamID", 0);
    r.setValue("StepID", 0);
    r.setValue("Time", QDateTime::currentDateTime());
    r.setValue("Filename", py.currentFilename);
    if (!db.dbModel->insertRecord(-1, r))
        qDebug() << db.dbModel->lastError();

    return db;
}
