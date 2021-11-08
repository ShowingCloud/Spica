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
    void writeSocket(const QByteArray &data);

private:
    QSharedMemory *memory;
    QThreadPool *pool;
    QLocalServer *server;
    QLocalSocket *socket = nullptr;
    pylon::CAM_POS position;
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
