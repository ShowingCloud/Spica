#include <iso646.h>
#include <QDebug>
#include <iso646.h>

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
    dbModel->setTable(DB_TABLES[DB_TBL_IMG]);
    dbModel->setFilter("CamID=" + QString::number(0));
    dbModel->setSort(3, Qt::DescendingOrder);
    dbModel->select();
    QSqlRecord r = dbModel->record(0);

    return r.value("Filename").value<QString>();
}

const QStringList database::getRecentImages(const int  num) const
{
    dbModel->setTable(DB_TABLES[DB_TBL_IMG]);
    dbModel->setFilter("CamID=" + QString::number(0));
    dbModel->setSort(3, Qt::DescendingOrder);
    dbModel->select();

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
    QStringList ret = {};
    for (const int n : num) {
        imgModel->setFilter("Id=" + QString::number(n));
        imgModel->select();
        QString file = imgModel->record(0).value("FileName").toString();
        if (file == "") file = "resources/empty.png";
        ret << file;
    }
    return ret;
}

const QStringList database::getAlgoDefects(const QVector<int> num) const
{
    QStringList ret = {};
    for (const int n : num) {
        algoModel->setFilter("Id=" + QString::number(n));
        algoModel->select();
        ret << algoModel->record(0).value("FileName").toString();
    }
    return ret;
}

const database &database::operator<< (int &lastId) const
{
    lastId = dbQuery.lastInsertId().toInt();
    return *this;
}
