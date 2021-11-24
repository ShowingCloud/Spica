#include <QFileInfo>
#include <iso646.h>

#include "frontend.h"
#include "database.h"

const QString frontend::getRecentImages()
{
    return "file:///" + QFileInfo(globalDB->getRecentImages()).absoluteFilePath();
}

const QStringList frontend::getRecentImages(const int num)
{
    QStringList list = globalDB->getRecentImages(num);
    return std::accumulate(list.begin(), list.end(), QStringList(), [](QStringList ret, const QString str) {
        return ret << "file:///" + QFileInfo(str).absoluteFilePath();
    });
}

const QStringList productRecordModel::getImages(const int row)
{
    QVector<int> imgs;
    QSqlRecord r = prodModel->record(row);
    selectedId = r.value("Id").toInt();

    beginResetModel();
    selectedRow = row;
    endResetModel();

    for (int i = 1; i < 9; ++i)
        imgs << r.value("Cam" + QString::number(i) + "Img").toInt();

    QStringList list = globalDB->getImages(imgs);
    return std::accumulate(list.begin(), list.end(), QStringList(), [](QStringList ret, const QString str) {
        return ret << "file:///" + QFileInfo(str).absoluteFilePath();
    });
}

void productRecordModel::fillData()
{
    beginResetModel();
    *this << *globalDB;
    endResetModel();
}

productRecordModel &operator<< (productRecordModel &model, const database &db)
{
    model.records = {};
    model.prodModel = db.prodModel;
    db.prodModel->select();

    model.selectedRow = -1;
    for (int i = 0; i < qMin(db.prodModel->rowCount(), 100); ++i) {
        QSqlRecord r = db.prodModel->record(i);
        if (r.value("Id").toString() == "")
            return model;

        if (r.value("Id").toInt() == model.selectedId) model.selectedRow = i;

        QStringList s = {};
        s << r.value("ProdId").toString();
        s << r.value("Time").toString();
        s << QString::number(r.value("PneuResult").toInt());
        for (int j = 1; j < 9; ++j)
            s << QString::number(r.value("Cam" + QString::number(j) + "Result").toInt());
        s << QString::number(r.value("Result").toInt());

        model.records << s;
    }

    return model;
}
