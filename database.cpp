#include <iso646.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QPoint>

#include "database.h"

database::database(QObject *parent) : QObject(parent)
{
    if (not QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

    if (db.isOpen()) db.close();
    db.setDatabaseName(database::filename);
    if (not db.open()) {
        qDebug() << "Error: Failed to connect db." << db.lastError();
    }

    dbModel = new QSqlTableModel(this, db);
    dbQuery = QSqlQuery(db);

    createTable();

    prodModel = new QSqlTableModel(this, db);
    prodModel->setTable(DB_TABLES[DB_TBL_PROD]);
    prodModel->setSort(0, Qt::DescendingOrder);

    imgModel = new QSqlTableModel(this, db);
    imgModel->setTable(DB_TABLES[DB_TBL_IMG]);
    imgModel->setSort(0, Qt::DescendingOrder);

    algoModel = new QSqlTableModel(this, db);
    algoModel->setTable(DB_TABLES[DB_TBL_ALGO]);
    algoModel->setSort(0, Qt::DescendingOrder);

    prefModel = new QSqlTableModel(this, db);
    prefModel->setTable(DB_TABLES[DB_TBL_PREF]);
    prefModel->setSort(0, Qt::DescendingOrder);
}

database::~database()
{
    db.close();
    qDebug() << "Database closed.";
}

bool database::createTable()
{
    for (const DB_TBL table : DB_TBL_ALL) {
        QString q = "CREATE TABLE IF NOT EXISTS ";
        q += DB_TABLES[table] + " (";

        for (const QStringList &column : DB_COLUMNS[table]) {
            q += column.join(" ") + ", ";
        }
        q.replace(q.length() - 2, 1, ")");
        dbQuery.prepare(q);
        if(!dbQuery.exec())
            qDebug() << dbQuery.lastError();

        for (const QString &column : DB_INDEXES[table]) {
            dbQuery.prepare("CREATE INDEX IF NOT EXISTS " + DB_TABLES[table] + "_" + column
                    + " ON " + DB_TABLES[table] + "(" + column + ")");
            if(!dbQuery.exec())
                qDebug() << dbQuery.lastError();
        }
    }
    return true;
}

const QString database::getRecentImages() const
{
    imgModel->setFilter("CamID=" + QString::number(0));
    imgModel->setSort(3, Qt::DescendingOrder);
    imgModel->select();
    QSqlRecord r = dbModel->record(0);

    return r.value("Filename").value<QString>();
}

const QStringList database::getRecentImages(const int  num) const
{
    imgModel->setFilter("CamID=" + QString::number(0));
    imgModel->setSort(3, Qt::DescendingOrder);
    imgModel->select();

    QStringList ret = {};
    for (int i = 0; i < std::min(dbModel->rowCount(), num); ++i) {
        QSqlRecord r = dbModel->record(i);
        if (r.value("Id").toString() != "") {
            ret << r.value("Filename").value<QString>();
        }
    }
    return ret;
}

const QStringList database::getImages(const QVector<int> num) const
{
    QStringList ret;
    for (const int n : num) {
        imgModel->setFilter("Id=" + QString::number(n));
        imgModel->select();
        QString file = imgModel->record(0).value("FileName").toString();
        if (file == "") file = "resources/empty.png";
        ret << file;
    }
    return ret;
}

const QVector<QVector<int>> database::getAlgo(const QVector<int> num) const
{
    QVector<QVector<int>> ret;
    for (const int n : num) {
        algoModel->setFilter("Id=" + QString::number(n));
        algoModel->select();

        QVector<int> algo;
        for (int i = 0; i < 3; ++i)
            algo << algoModel->record(0).value("Rslt" + QString::number(i + 1)).toInt();
        ret << algo;
    }
    return ret;
}

const QVector<QVector<QVector<QPoint>>> database::getAlgoImg(const QVector<int> num) const
{
    QVector<QVector<QVector<QPoint>>> ret;
    for (const int n : num) {
        algoModel->setFilter("Id=" + QString::number(n));
        algoModel->select();

        QVector<QVector<QPoint>> algoImg;
        for (int i = 0; i < 3; ++i) {
            QJsonArray arr = QJsonDocument::fromJson(
                        algoModel->record(0).value("Rslt" + QString::number(i + 1) + "Img").toByteArray()).array();
            algoImg << std::accumulate(arr.cbegin(), arr.cend(), QVector<QPoint>(), [](QVector<QPoint> img, const QJsonValue point) {
                       return img << QPoint(point.toArray()[0].toInt(), point.toArray()[1].toInt());
            });
        }
        ret << algoImg;
    }
    return ret;
}

const QVector<QVector<QVector<QVector<QPoint>>>> database::getAlgoAreas(const QVector<int> num) const
{
    QVector<QVector<QVector<QVector<QPoint>>>> ret;
    for (const int n : num) {
        algoModel->setFilter("Id=" + QString::number(n));
        algoModel->select();

        QVector<QVector<QVector<QPoint>>> algoAreas;
        for (int i = 0; i < 3; ++i) {
            QJsonArray arr = QJsonDocument::fromJson(
                        algoModel->record(0).value("Rslt" + QString::number(i + 1) + "Areas").toByteArray()).array();
            algoAreas << std::accumulate(arr.cbegin(), arr.cend(), QVector<QVector<QPoint>>(),
                        [](QVector<QVector<QPoint>> areas, const QJsonValue area) {
                        return areas << std::accumulate(area.toArray().cbegin(), area.toArray().cend(), QVector<QPoint>(),
                            [](QVector<QPoint> points, const QJsonValue point) {
                            return points << QPoint(point.toArray()[0].toInt(), point.toArray()[1].toInt());
                        });
            });
        }
        ret << algoAreas;
    }
    return ret;
}

const QVector<QVector<QVector<int>>> database::getAlgoDefects(const QVector<int> num) const
{
    QVector<QVector<QVector<int>>> ret;
    for (const int n : num) {
        algoModel->setFilter("Id=" + QString::number(n));
        algoModel->select();

        QVector<QVector<int>> algoDefs;
        for (int i = 0; i < 3; ++i) {
            QJsonArray arr = QJsonDocument::fromJson(
                        algoModel->record(0).value("Rslt" + QString::number(i + 1) + "Defs").toByteArray()).array();
            algoDefs << std::accumulate(arr.cbegin(), arr.cend(), QVector<int>(), [](QVector<int> v, const QJsonValue val) {
                        return v << val.toInt();
            });
        }
        ret << algoDefs;
    }
    return ret;
}

const QJsonDocument database::getPref(const QString var) const
{
    prefModel->setFilter("Var=" + var);
    prefModel->select();
    return QJsonDocument::fromJson(prefModel->record(0).value("Value").toByteArray());
}

bool database::setPref(const QString var, const QJsonDocument value)
{
    prefModel->setFilter("Var=" + var);
    prefModel->select();

    QSqlRecord r;
    if (prefModel->rowCount() != 0)
        r = prefModel->record(0);
    else
        r = prefModel->record();

    r.setValue("Time", QDateTime::currentDateTime());
    r.setValue("Value", value.toJson(QJsonDocument::Compact));

    bool ret;
    if (prefModel->rowCount() != 0)
        ret = prefModel->setRecord(0, r);
    else
        ret = prefModel->insertRecord(-1, r);

    if (!ret) {
        qDebug() << prefModel->lastError();
        return false;
    }

    return true;
}

const database &database::operator<< (int &lastId) const
{
    lastId = dbQuery.lastInsertId().toInt();
    return *this;
}
