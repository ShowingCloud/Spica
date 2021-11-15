#ifndef DATABASE_H
#define DATABASE_H

#include <QCoreApplication>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>
#include <iso646.h>

class pylon;
class algorithm;
class product;

class database : public QObject
{
    Q_OBJECT

public:
    explicit database(QObject *parent = nullptr);
    ~database() override;

    friend database &operator<< (database &db, const pylon &py);
    friend database &operator<< (database &db, const algorithm &algo);
    friend database &operator<< (database &db, const product &prod);
    const database &operator<< (int &lastId) const;

    const QString getRecentImages() const;
    const QStringList getRecentImages(const int  num) const;

    enum DB_TBL { DB_TBL_IMG, DB_TBL_ALGO, DB_TBL_PROD };
    static const inline DB_TBL DB_TBL_ALL[] = { DB_TBL_IMG, DB_TBL_ALGO, DB_TBL_PROD };
    enum DB_RET { DB_RET_SUCCESS };

private:
    bool prepare();
    bool createTable();

    QSqlDatabase    db          = QSqlDatabase();
    QSqlTableModel  *dbModel    = nullptr;
    QSqlQuery       dbQuery     = QSqlQuery();
    QString         setDBTable  = QString();

    inline const static QString filename = "database.db";

    inline static const QHash<DB_TBL, QString> DB_TABLES = {
        {DB_TBL_IMG, "images"}, {DB_TBL_ALGO, "algorithms"}, {DB_TBL_PROD, "products"}};

    static const inline QHash<DB_TBL, QList<QStringList>> DB_COLUMNS = {
        {DB_TBL_IMG, {{"Id", "INTEGER", "UNIQUE", "PRIMARY KEY", "AUTOINCREMENT"},
                      {"CamID", "INTEGER"}, {"Position", "INTEGER"}, {"Time", "DATETIME"},
                      {"Filename", "TEXT"}}},
        {DB_TBL_ALGO, {{"Id", "INTEGER", "UNIQUE", "PRIMARY KEY", "AUTOINCREMENT"},
                       {"ImgID", "INTEGER"}, {"ResultJSON", "TEXT"}, {"TIme", "DATETIME"},
                       {"Result1", "INTEGER"}, {"Result2", "INTEGER"}, {"Result3", "INTEGER"}}},
        {DB_TBL_PROD, {{"Id", "INTEGER", "UNIQUE", "PRIMARY KEY", "AUTOINCREMENT"},
                       {"ProdId", "INTEGER"}, {"PneuResult", "INTEGER"}, {"Time", "DATETIME"},
                       {"Cam1Img", "INTEGER"}, {"Cam1Algo", "INTEGER"}, {"Cam1Result", "INTEGER"},
                       {"Cam2Img", "INTEGER"}, {"Cam2Algo", "INTEGER"}, {"Cam2Result", "INTEGER"},
                       {"Cam3Img", "INTEGER"}, {"Cam3Algo", "INTEGER"}, {"Cam3Result", "INTEGER"},
                       {"Cam4Img", "INTEGER"}, {"Cam4Algo", "INTEGER"}, {"Cam4Result", "INTEGER"},
                       {"Cam5Img", "INTEGER"}, {"Cam5Algo", "INTEGER"}, {"Cam5Result", "INTEGER"},
                       {"Cam6Img", "INTEGER"}, {"Cam6Algo", "INTEGER"}, {"Cam6Result", "INTEGER"},
                       {"Cam7Img", "INTEGER"}, {"Cam7Algo", "INTEGER"}, {"Cam7Result", "INTEGER"},
                       {"Cam8Img", "INTEGER"}, {"Cam8Algo", "INTEGER"}, {"Cam8Result", "INTEGER"},
                       {"Result", "INTEGER"}, {"LCR", "INTEGER"}}}};

    inline static const QHash<DB_TBL, QStringList> DB_INDEXES = {
        {DB_TBL_IMG, {"CamID", "Position", "Time"}},
        {DB_TBL_ALGO, {"ImgID", "Time", "Result"}},
        {DB_TBL_PROD, {"ProdId", "Time", "Result"}}};

signals:

public slots:
};

extern database *globalDB;

#endif // DATABASE_H
