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

private:
    QSharedMemory *memory;
    QThreadPool *pool;
    QLocalServer *server;
    QLocalSocket *socket = nullptr;
    pylon::CAM_POS position;
};

#endif // ALGORITHM_H
