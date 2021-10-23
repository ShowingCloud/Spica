#ifndef FRONTEND_H
#define FRONTEND_H

#undef interface

#include <QCoreApplication>
#include <QtQml>

class frontend : public QObject
{
    Q_OBJECT
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
    QML_ELEMENT
#endif

public:
    explicit frontend(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    static const QString getRecentImages();
    static const QStringList getRecentImages(const int num);
};

#endif // FRONTEND_H
