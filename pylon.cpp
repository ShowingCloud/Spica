#include <QTimer>
#include <QImage>
#include <QtDebug>
#include <QDir>
#include <QDateTime>
#include <QUuid>
#include <QJsonArray>
#include <QJsonDocument>
#include <iso646.h>

#include "pylon.h"
#include "database.h"
#include "algorithm.h"

pylon::pylon(const Pylon::CDeviceInfo dev, QObject *parent)
    : QObject(parent), camera(Pylon::CTlFactory::GetInstance().CreateDevice(dev))
{
    qDebug() << "Using device: " << camera.GetDeviceInfo().GetModelName();
    camera.MaxNumBuffer = 100;
/*
    camera.RegisterConfiguration(new Pylon::CSoftwareTriggerConfiguration, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);
    camera.RegisterImageEventHandler(new pylonImageEventHandler, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
    camera.Open();

    if (camera.CanWaitForFrameTriggerReady())
        camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
*/}

pylon::~pylon()
{
    camera.DestroyDevice();
    qDebug() << "Destroyed" << this;
    delete algo;
}

void pylon::initialize(QObject *parent)
{
    QDir::current().mkdir(savePath);
    idPosition = {};
    positionStation = {};
    QJsonDocument pylonMap = globalDB->getPref("PylonMap");
    if (pylonMap == QJsonDocument()) pylonMap = defaultIdPosition;
    QJsonDocument positionMap = globalDB->getPref("PositionMap");
    if (positionMap == QJsonDocument()) positionMap = defaultPositionStation;

    // For non-existing pylon dev id, dev not created and idPosition
    // item missing but positionStation should be fully connected

    for (const QJsonValue obj : positionMap["positionStation"].toArray())
        positionStation.insert(static_cast<pylon::CAM_POS>(obj["position"].toInt()),
                static_cast<devPLC::CAM_POS>(obj["station"].toInt()));

    Pylon::PylonInitialize();
    Pylon::DeviceInfoList pylonList;
    if (Pylon::CTlFactory::GetInstance().EnumerateDevices(pylonList) != 0)
        for (const Pylon::CDeviceInfo &dev : qAsConst(pylonList))
            for (const QJsonValue obj : pylonMap["idPosition"].toArray())
                if (obj["id"].toInt() == std::stoi(dev.GetSerialNumber().c_str())) {
                    pylon *p = new pylon(dev, parent);
                    devList << p;
                    p->position = static_cast<pylon::CAM_POS>(obj["position"].toInt());
                    idPosition.insert(obj["id"].toInt(), p->position);

                    p->station = positionStation[p->position];
                    posDevList[p->position] = p;
                    p->algo = new algorithm(p->position, p, parent);
                    qDebug() << p << p->position << p->station;
                }
}

void pylon::destroy()
{
    for (pylon *p : qAsConst(devList))
        delete p;
    Pylon::PylonTerminate();
}

const QVariant pylon::getCameraInfo()
{
    QVector<QVariantMap> arr;
    Pylon::DeviceInfoList pylonList;
    if (Pylon::CTlFactory::GetInstance().EnumerateDevices(pylonList) != 0)
        for (const Pylon::CDeviceInfo &dev : qAsConst(pylonList)) {
            QVariantMap obj;
            obj["serialNumber"] = dev.GetSerialNumber().c_str();
            obj["address"] = dev.GetAddress().c_str();
            obj["friendlyName"] = dev.GetFriendlyName().c_str();
            obj["macAddress"] = dev.GetMacAddress().c_str();
            arr << obj;
        }

    return QVariant::fromValue(arr);
}

int pylon::capture()
{
    camera.GrabOne(5000, result);
//    if (camera.WaitForFrameTriggerReady(1000, Pylon::TimeoutHandling_Return))
//        camera.ExecuteSoftwareTrigger();

    if (result->GrabSucceeded()) {
        currentImage = static_cast<const quint8 *>(result->GetBuffer());
        currentImageSize = static_cast<int>(result->GetBufferSize());
        currentImageWidth = static_cast<int>(result->GetWidth());
        currentImageHeight = static_cast<int>(result->GetHeight());
        currentFilename = savePath + "/" + QDateTime::currentDateTime().toString(Qt::ISODate) + "-"
                                        + QUuid::createUuid().toString(QUuid::Id128) + ".png";
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
