#include <QDebug>
#include <QCryptographicHash>
#include <QDateTime>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <iso646.h>

#include "algorithm.h"

algorithm::algorithm(const pylon::CAM_POS pos, QObject *parent) : QObject(parent)
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
            QJsonDocument resp = QJsonDocument::fromJson(socket->readAll());
            qDebug() << "Got response: " << resp;
        });
    });
    QLocalServer::removeServer("algo-" + QString::number(position));
    if (not server->listen("algo-" + QString::number(position)))
        qDebug() << "Error listening: " << server->errorString();
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
        QJsonObject packet;
        packet["id"] = 0;
        packet["size"] = cam.currentImageSize;
        packet["height"] = cam.currentImageHeight;
        packet["width"] = cam.currentImageWidth;
        packet["filename"] = cam.currentFilename;
        algo.socket->write(QJsonDocument(packet).toJson());

        if (algo.pool->activeThreadCount() == 0) {
            sharedRunner *run = new sharedRunner(algo.memory, cam.currentImage, cam.currentImageSize);
            algo.pool->start(run);
        }
    }

    return algo;
}

void sharedRunner::run()
{
    memory->lock();
    memcpy(memory->data(), camdata, static_cast<size_t>(camsize));

    qWarning() << "Wrote shared memory: " << memory->key() << QCryptographicHash::hash(
                    QByteArray(static_cast<const char *>(memory->constData()), camsize),
                    QCryptographicHash::Md5).toHex();

    memory->unlock();
}
