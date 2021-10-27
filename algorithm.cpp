#include <QDebug>
#include <QCryptographicHash>
#include <QDateTime>
#include <QLocalSocket>
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
            qDebug() << "Wait for connected timeout";
            return;
        }
        connect(socket, &QLocalSocket::readyRead, [=](){
            qDebug() << socket->readAll();
        });
    });
    server->listen("algo-" + QString::number(position));
}

algorithm &operator<< (algorithm &algo, const pylon &cam)
{
    if (algo.memory->isAttached() and algo.memory->size() != cam.currentImageSize) {
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
        algo.socket->write("test data");
        if (algo.pool->activeThreadCount() < 1) {
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

    qDebug() << "Wrote shared memory: " << memory->key() << QCryptographicHash::hash(
                    QByteArray(static_cast<const char *>(memory->data()), camsize),
                    QCryptographicHash::Md5).toHex();

    memory->unlock();

}
