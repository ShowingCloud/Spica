#ifndef FRONTEND_H
#define FRONTEND_H

#undef interface

#include <QCoreApplication>
#include <QtQml>
#include <QColor>

#include "database.h"

class frontend : public QObject
{
    Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    QML_ELEMENT
#endif

public:
    explicit frontend(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    static const QString getRecentImages();
    static const QStringList getRecentImages(const int num);
};


class productRecordModel : public QAbstractTableModel
{
    Q_OBJECT
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
    QML_ELEMENT
#endif

public:
    explicit productRecordModel(QAbstractTableModel *parent = nullptr) : QAbstractTableModel(parent) { fillData(); }

    friend productRecordModel &operator<< (productRecordModel &model, const database &db);

    inline int rowCount(const QModelIndex & = QModelIndex()) const override {
        return records.length();
    }

    inline int columnCount(const QModelIndex & = QModelIndex()) const override {
        return 10;
    }

    inline QVariant data(const QModelIndex &index, const int role) const override {
        switch (role) {
        case Qt::DisplayRole:
            return records[index.row()][index.column()];
        case Qt::ForegroundRole:
            return QColor("black");
        case Qt::TextAlignmentRole: {
            return Qt::AlignHCenter;
        } case Qt::SizeHintRole: {
            if (index.column() == 0)
                return 50;
            else if (index.column() == 1)
                return 120;
            else
                return 20;
        } default:
            break;
        }
        return QVariant();
    }

    inline QVariant headerData(const int section, const Qt::Orientation orientation, const int role) const override {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            static const QStringList header = { tr("Id"), tr("Time"), tr("Pneu Result"), tr("Camera") + " 1 " + tr("Result"),
                                                tr("Camera") + " 2 " + tr("Result"),
                                                tr("Camera") + " 3 " + tr("Result"),
                                                tr("Camera") + " 4 " + tr("Result"),
                                                tr("Camera") + " 5 " + tr("Result"),
                                                tr("Camera") + " 6 " + tr("Result"),
                                                tr("Camera") + " 7 " + tr("Result"),
                                                tr("Camera") + " 8 " + tr("Result"),
                                                tr("Overall Result") };
            return header[section];
        }
        return QVariant();
    }

    inline QHash<int, QByteArray> roleNames() const override {
        return QHash<int, QByteArray> {
            { Qt::DisplayRole, "display" },
            { Qt::ForegroundRole, "foreground" },
            { Qt::TextAlignmentRole, "textalignment" },
            { Qt::SizeHintRole, "sizehint" }
        };
    }

public slots:
    void fillData();
    const QStringList getImages(const int row);

private:
    QList<QStringList> records;
    QSqlTableModel *prodModel;
};

#endif // FRONTEND_H
