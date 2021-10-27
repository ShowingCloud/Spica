#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include <QSharedMemory>
#include <QRunnable>
#include <QThreadPool>
#include <QLocalServer>

#include "pylon.h"

class algorithm : public QObject
{
    Q_OBJECT
public:
    explicit algorithm(const pylon::CAM_POS pos, QObject *parent = nullptr);

    friend algorithm &operator<< (algorithm &algo, const pylon &cam);

signals:

private:
    QSharedMemory *memory;
    QThreadPool *pool;
    QLocalServer *server;
    QLocalSocket *socket = nullptr;
    pylon::CAM_POS position;
};

class sharedRunner : public QRunnable
{
public:
    explicit sharedRunner(QSharedMemory *memory, const quint8 *camdata, const int camsize)
        : memory(memory), camdata(camdata), camsize(camsize) {}

private:
    QSharedMemory *memory;
    const quint8 *camdata;
    const int camsize;

    void run() override;
};

#endif // ALGORITHM_H
