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
class productRecordModel;

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
    friend productRecordModel &operator<< (productRecordModel &model, const database &db);

    const QString getRecentImages() const;
    const QStringList getRecentImages(const int  num) const;
    const QStringList getImages(const QVector<int> num) const;
    const QStringList getAlgoDefects(const QVector<int> num) const;

    enum DB_TBL { DB_TBL_IMG, DB_TBL_ALGO, DB_TBL_PROD };
    static const inline DB_TBL DB_TBL_ALL[] = { DB_TBL_IMG, DB_TBL_ALGO, DB_TBL_PROD };
    enum DB_RET { DB_RET_SUCCESS };

private:
    bool prepare();
    bool createTable();

    QSqlDatabase    db          = QSqlDatabase();
    QSqlTableModel  *dbModel    = nullptr;
    QSqlQuery       dbQuery     = QSqlQuery();
    QSqlTableModel  *prodModel  = nullptr;
    QSqlTableModel  *imgModel   = nullptr;
    QSqlTableModel  *algoModel  = nullptr;

    inline const static QString filename = "database.db";

    inline static const QHash<DB_TBL, QString> DB_TABLES = {
        {DB_TBL_IMG, "images"}, {DB_TBL_ALGO, "algorithms"}, {DB_TBL_PROD, "products"}};

    static const inline QHash<DB_TBL, QList<QStringList>> DB_COLUMNS = {
        {DB_TBL_IMG, {{"Id", "INTEGER", "UNIQUE", "PRIMARY KEY", "AUTOINCREMENT"},
                      {"CamID", "INTEGER"}, {"Position", "INTEGER"}, {"Time", "DATETIME"},
                      {"Filename", "TEXT"}}},
        {DB_TBL_ALGO, {{"Id", "INTEGER", "UNIQUE", "PRIMARY KEY", "AUTOINCREMENT"},
                       {"ImgID", "INTEGER"}, {"RsltJSON", "TEXT"}, {"Time", "DATETIME"},
                       {"Rslt1", "INTEGER"}, {"Rslt1Img", "TEXT"}, {"Rslt1Areas", "TEXT"}, {"Rslt1Defs", "TEXT"},
                       {"Rslt2", "INTEGER"}, {"Rslt2Img", "TEXT"}, {"Rslt2Areas", "TEXT"}, {"Rslt2Defs", "TEXT"},
                       {"Rslt3", "INTEGER"}, {"Rslt3Img", "TEXT"}, {"Rslt3Areas", "TEXT"}, {"Rslt3Defs", "TEXT"}}},
        {DB_TBL_PROD, {{"Id", "INTEGER", "UNIQUE", "PRIMARY KEY", "AUTOINCREMENT"},
                       {"ProdId", "INTEGER"}, {"PneuRslt", "INTEGER"}, {"Time", "DATETIME"},
                       {"Cam1Img", "INTEGER"}, {"Cam1Algo", "INTEGER"}, {"Cam1Rslt", "INTEGER"},
                       {"Cam2Img", "INTEGER"}, {"Cam2Algo", "INTEGER"}, {"Cam2Rslt", "INTEGER"},
                       {"Cam3Img", "INTEGER"}, {"Cam3Algo", "INTEGER"}, {"Cam3Rslt", "INTEGER"},
                       {"Cam4Img", "INTEGER"}, {"Cam4Algo", "INTEGER"}, {"Cam4Rslt", "INTEGER"},
                       {"Cam5Img", "INTEGER"}, {"Cam5Algo", "INTEGER"}, {"Cam5Rslt", "INTEGER"},
                       {"Cam6Img", "INTEGER"}, {"Cam6Algo", "INTEGER"}, {"Cam6Rslt", "INTEGER"},
                       {"Cam7Img", "INTEGER"}, {"Cam7Algo", "INTEGER"}, {"Cam7Rslt", "INTEGER"},
                       {"Cam8Img", "INTEGER"}, {"Cam8Algo", "INTEGER"}, {"Cam8Rslt", "INTEGER"},
                       {"Rslt", "INTEGER"}, {"LCR", "INTEGER"}}}};

    inline static const QHash<DB_TBL, QStringList> DB_INDEXES = {
        {DB_TBL_IMG, {"CamID", "Position", "Time"}},
        {DB_TBL_ALGO, {"ImgID", "Time", "Rslt1", "Rslt2", "Rslt3"}},
        {DB_TBL_PROD, {"ProdId", "Time", "Rslt"}}};

signals:

public slots:
};

extern database *globalDB;

#endif // DATABASE_H
