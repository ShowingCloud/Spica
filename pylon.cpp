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

    algo = new algorithm(position, this, parent);
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
#ifdef QT_DEBUG
        for (int i = 0; i < 8; ++i) {
            Pylon::CDeviceInfo dev = pylonList[0];
#else
        for (const Pylon::CDeviceInfo &dev : qAsConst(pylonList)) {
#endif
            pylon *p = new pylon(dev, parent);
            devList << p;

#ifdef QT_DEBUG
            p->position = idPosition[i];
#else
            p->position = idPosition[0];
#endif
            p->station = positionStation[p->position];
            posDevList[p->position] = p;
            //devPLC::addDeviceList(p->station, p);
            qDebug() << p << p->position << p->station;
        }
}

void pylon::destroy()
{
    for (pylon *p : qAsConst(devList))
        delete p;
    Pylon::PylonTerminate();
}

int pylon::capture()
{
    camera.GrabOne(5000, result);

    if (result->GrabSucceeded()) {
        currentImage = static_cast<const quint8 *>(result->GetBuffer());
        currentImageSize = static_cast<int>(result->GetBufferSize());
        currentImageWidth = static_cast<int>(result->GetWidth());
        currentImageHeight = static_cast<int>(result->GetHeight());
        currentFilename = savePath + "/" + QDateTime::currentDateTime().toString(Qt::ISODate) + "-"
                                        + QUuid::createUuid().toString(QUuid::Id128) + ".png";
        qDebug() << currentImageSize << currentImageWidth << currentImageHeight << currentFilename;
        /*
        QImage img(static_cast<const quint8 *>(result->GetBuffer()),
                   static_cast<int>(result->GetWidth()),
                   static_cast<int>(result->GetHeight()),
                   QImage::Format_RGB888);
        */

        Pylon::CImagePersistence::Save(Pylon::EImageFileFormat::ImageFileFormat_Png,
                                       currentFilename.replace(":", "-").toStdString().c_str(), result);
        *globalDB << *this << this->imgId;

        *algo << *this;
        return this->imgId;
    } else {
        qDebug() << "Error: " << QString::number(result->GetErrorCode(), 16) << " " <<
                    result->GetErrorDescription();
        return -1;
    }
}

database &operator<< (database &db, const pylon &py)
{
    db.dbModel->setTable(db.DB_TABLES[db.DB_TBL_IMG]);
    QSqlRecord r = db.dbModel->record();
    r.setValue("CamID", 0);
    r.setValue("Position", py.position);
    r.setValue("Time", QDateTime::currentDateTime());
    r.setValue("Filename", py.currentFilename);
    if (!db.dbModel->insertRecord(-1, r))
        qDebug() << db.dbModel->lastError();

    return db;
}
