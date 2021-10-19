#ifndef FRONTEND_H
#define FRONTEND_H

#include <QCoreApplication>

class frontend : public QObject
{
    Q_OBJECT

public:
    explicit frontend(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    static const QString getRecentImages();
    static const QStringList getRecentImages(const int num);
};

#endif // FRONTEND_H
