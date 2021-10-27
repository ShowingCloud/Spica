#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include <QSharedMemory>

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
    pylon::CAM_POS position;
};

#endif // ALGORITHM_H
