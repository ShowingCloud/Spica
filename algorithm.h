#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include <QSharedMemory>
#include <QRunnable>
#include <QThreadPool>
#include <QLocalServer>
#include <iso646.h>

#include "pylon.h"

class algorithm : public QObject
{
    Q_OBJECT

public:
    explicit algorithm(const pylon::CAM_POS pos, const pylon *cam, QObject *parent = nullptr);

    friend database &operator<< (database &db, const algorithm &algo);
    friend algorithm &operator<< (algorithm &algo, const pylon &cam);

signals:
    void writeSocket(const QByteArray &data);

private:
    QSharedMemory *memory;
    QThreadPool *pool;
    QLocalServer *server;
    QLocalSocket *socket = nullptr;
    pylon::CAM_POS position;
    int imgId;
    int algoId;
    QString resultJSON;
    QVector<int> result = {0, 0, 0};
    QVector<QVector<QPair<int, int>>> resultImg = {{}, {}, {}};
    QVector<QVector<QVector<QPair<int, int>>>> resultAreas = {{}, {}, {}};
    QVector<QVector<int>> resultDefs = {{}, {}, {}};
};

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
class FunctionRunnable : public QRunnable
{
    std::function<void()> m_functionToRun;

public:
    FunctionRunnable(std::function<void()> functionToRun)
        : m_functionToRun(std::move(functionToRun)) {}
    inline void run() override {
        m_functionToRun();
    }
};
#endif

#endif // ALGORITHM_H
