#include <QDebug>
#include <QCryptographicHash>
#include <QDateTime>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iso646.h>

#include "algorithm.h"
#include "database.h"

algorithm::algorithm(const pylon::CAM_POS pos, const pylon *cam, QObject *parent) : QObject(parent)
{
    position = pos;
    memory = new QSharedMemory("algo_" + QString::number(position), this);
    pool = new QThreadPool(this);

    server = new QLocalServer(this);
    connect(server, &QLocalServer::newConnection, [=](){
        socket = server->nextPendingConnection();
        if (not socket->waitForConnected()) {
            qDebug() << "Socket wait-for-connected timeout";
            return;
        }
        connect(socket, &QLocalSocket::disconnected, [=](){
            qWarning() << "Socket disconnected";
            socket->deleteLater();
            socket = nullptr;
        });
        connect(socket, &QLocalSocket::readyRead, [=](){
            const QJsonDocument resp = QJsonDocument::fromJson(socket->readAll());
            result = {0, 0, 0};
            imgId = resp["id"].toInt();
            resultJSON = resp.toJson(QJsonDocument::Compact);

            for (const QJsonValueRef p : resp["results"].toArray()) {
                QJsonObject prod = p.toObject();
                int id = prod["id"].toInt();
                for (const QJsonValueRef r : prod["location"].toArray())
                    resultImg[id] << QPair(r.toArray()[0].toInt(), r.toArray()[1].toInt());

                result[id] = not prod["have_defects"].toBool();
                if (not result[id]) {
                    for (const QJsonValueRef r : prod["defects"].toArray()) {
                        resultDefs[id] << r.toObject()["type"].toInt();

                        QVector<QPair<int, int>> area;
                        for (const QJsonValueRef s : r.toObject()["pos"].toArray())
                            area << QPair(s.toArray()[0].toInt(), s.toArray()[1].toInt());
                        resultAreas[id] << area;
                    }
                }
            }

            *globalDB << *this << this->algoId;

            emit cam->gotAlgo(imgId, this->algoId, result);
            qDebug() << "Got response: " << resp;
        });
    });
    QLocalServer::removeServer("algo-" + QString::number(position));
    if (not server->listen("algo-" + QString::number(position)))
        qDebug() << "Error listening: " << server->errorString();

    connect(this, &algorithm::writeSocket, this, [&](const QByteArray &data){ socket->write(data); });
}

algorithm &operator<< (algorithm &algo, const pylon &cam)
{
    if (algo.memory->isAttached() and algo.memory->size() < cam.currentImageSize) {
        delete algo.memory;
        algo.memory = new QSharedMemory("algo_" + QString::number(algo.position), &algo);
    }

    if (not algo.memory->isAttached())
        if (not algo.memory->attach(QSharedMemory::ReadWrite))
            if (not algo.memory->create(cam.currentImageSize, QSharedMemory::ReadWrite)) {
                qDebug() << algo.memory->error() << algo.memory->errorString();
                return algo;
            }

    if (algo.socket != nullptr && algo.socket->isOpen()) {
        if (algo.pool->activeThreadCount() == 0) {
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
            algo.pool->start(new FunctionRunnable([&](){
#else
            algo.pool->start(([&](){
#endif
                algo.memory->lock();
                memcpy(algo.memory->data(), cam.currentImage, static_cast<size_t>(cam.currentImageSize));

                qWarning() << "Wrote shared algo.memory: " << algo.memory->key() << QCryptographicHash::hash(
                                  QByteArray(static_cast<const char *>(algo.memory->constData()), cam.currentImageSize),
                                  QCryptographicHash::Md5).toHex();

                algo.memory->unlock();

                QJsonObject packet;
                packet["id"] = cam.imgId;
                packet["size"] = cam.currentImageSize;
                packet["height"] = cam.currentImageHeight;
                packet["width"] = cam.currentImageWidth;
                packet["filename"] = cam.currentFilename;

                if (algo.socket && algo.socket->isOpen())
                    emit algo.writeSocket(QJsonDocument(packet).toJson(QJsonDocument::Compact));
            }));
        }
    }

    return algo;
}

database &operator<< (database &db, const algorithm &algo)
{
    db.dbModel->setTable(db.DB_TABLES[db.DB_TBL_ALGO]);
    QSqlRecord r = db.dbModel->record();
    r.setValue("ImgID", algo.imgId);
    r.setValue("RsltJSON", algo.resultJSON);
    for (int i = 0; i < 3; ++i) {
        r.setValue("Rslt" + QString::number(i + 1), algo.result[i]);
        QJsonArray img, areas, defs;
        for (const QPair<int, int> &l : algo.resultImg[i])
            img << (QJsonArray() << l.first << l.second);
        r.setValue("Rslt" + QString::number(i + 1) + "Img", QJsonDocument(img).toJson(QJsonDocument::Compact));

        for (const QVector<QPair<int, int>> &l : algo.resultAreas[i]) {
            QJsonArray a;
            for (const QPair<int, int> &p : l)
                a << (QJsonArray() << p.first << p.second);
            areas << a;
        }
        r.setValue("Rslt" + QString::number(i + 1) + "Areas", QJsonDocument(areas).toJson(QJsonDocument::Compact));

        for (const int d : algo.resultDefs[i])
            defs << d;
        r.setValue("Rslt" + QString::number(i + 1) + "Defs", QJsonDocument(defs).toJson(QJsonDocument::Compact));
    }
    r.setValue("Time", QDateTime::currentDateTime());
    if (!db.dbModel->insertRecord(-1, r))
        qDebug() << db.dbModel->lastError();

    return db;
}
