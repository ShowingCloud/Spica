#include <QDebug>
#include <QCryptographicHash>

#include "algorithm.h"

algorithm::algorithm(const pylon::CAM_POS pos, QObject *parent) : QObject(parent)
{
    position = pos;
    memory = new QSharedMemory("algo_" + QString::number(position), this);
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

    algo.memory->lock();
    memcpy(algo.memory->data(), cam.currentImage, static_cast<size_t>(cam.currentImageSize));

    qDebug() << "Wrote shared memory: " << algo.memory->key() << QCryptographicHash::hash(
                    QByteArray(static_cast<const char *>(algo.memory->data()), cam.currentImageSize),
                    QCryptographicHash::Md5).toHex();

    algo.memory->unlock();

    return algo;
}
