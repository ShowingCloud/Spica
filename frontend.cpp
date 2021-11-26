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
    return std::accumulate(list.cbegin(), list.cend(), QStringList(),
        [](QStringList ret, const QString str) {
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
    return std::accumulate(list.cbegin(), list.cend(), QStringList(),
        [](QStringList ret, const QString str) {
        return ret << "file:///" + QFileInfo(str).absoluteFilePath();
    });
}

const QVector<int> productRecordModel::getAlgoResults(const int row)
{
    QVector<int> algos;
    QSqlRecord r = prodModel->record(row);
    product::POS_LCR lcr = r.value("LCR").value<product::POS_LCR>();

    for (int i = 1; i < 9; ++i)
        algos << r.value("Cam" + QString::number(i) + "Algo").toInt();

    QVector<QVector<int>> results = globalDB->getAlgo(algos);
    return std::accumulate(results.cbegin(), results.cend(), QVector<int>(),
        [lcr](QVector<int> ret, const QVector<int> result) { return ret << result[lcr]; });
}

const QVariant productRecordModel::getAlgoImg(const int row)
{
    QVector<int> algos;
    QSqlRecord r = prodModel->record(row);
    product::POS_LCR lcr = r.value("LCR").value<product::POS_LCR>();

    for (int i = 1; i < 9; ++i)
        algos << r.value("Cam" + QString::number(i) + "Algo").toInt();

    QVector<QVector<QVector<QPoint>>> results = globalDB->getAlgoImg(algos);
    return QVariant::fromValue(std::accumulate(results.cbegin(), results.cend(), QVector<QVector<QPoint>>(),
        [lcr](QVector<QVector<QPoint>> ret, const QVector<QVector<QPoint>> result) {
        return ret << result[lcr];
    }));
}

const QVariant productRecordModel::getAlgoAreas(const int row)
{
    QVector<int> algos;
    QSqlRecord r = prodModel->record(row);
    product::POS_LCR lcr = r.value("LCR").value<product::POS_LCR>();

    for (int i = 1; i < 9; ++i)
        algos << r.value("Cam" + QString::number(i) + "Algo").toInt();

    QVector<QVector<QVector<QVector<QPoint>>>> results = globalDB->getAlgoAreas(algos);
    return QVariant::fromValue(std::accumulate(results.cbegin(), results.cend(), QVector<QVector<QVector<QPoint>>>(),
        [lcr](QVector<QVector<QVector<QPoint>>> ret, const QVector<QVector<QVector<QPoint>>> result) {
        return ret << result[lcr];
    }));
}

const QVariant productRecordModel::getAlgoDefects(const int row)
{
    QVector<int> algos;
    QSqlRecord r = prodModel->record(row);
    product::POS_LCR lcr = r.value("LCR").value<product::POS_LCR>();

    for (int i = 1; i < 9; ++i)
        algos << r.value("Cam" + QString::number(i) + "Algo").toInt();

    QVector<QVector<QVector<int>>> results = globalDB->getAlgoDefects(algos);
    return QVariant::fromValue(std::accumulate(results.cbegin(), results.cend(), QVector<QVector<int>>(),
        [lcr](QVector<QVector<int>> ret, const QVector<QVector<int>> result) { return ret << result[lcr]; }));
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
        s << QString::number(r.value("PneuRslt").toInt());
        for (int j = 1; j < 9; ++j)
            s << QString::number(r.value("Cam" + QString::number(j) + "Rslt").toInt());
        s << QString::number(r.value("Rslt").toInt());

        model.records << s;
    }

    return model;
}
