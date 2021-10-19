#ifndef DATABASE_H
#define DATABASE_H

#include <QCoreApplication>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>

class pylon;

class database : public QObject
{
    Q_OBJECT

public:
    explicit database(QObject *parent = nullptr);
    ~database() override;

    friend database &operator<< (database &db, const pylon &py);

    const QString getRecentImages() const;
    const QStringList getRecentImages(const int  num) const;

    enum DB_TBL { DB_TBL_IMG };
    static const inline DB_TBL DB_TBL_ALL[] = { DB_TBL_IMG };
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
        {DB_TBL_IMG, "images"}};

    static const inline QHash<DB_TBL, QList<QStringList>> DB_COLUMNS = {
        {DB_TBL_IMG, {{"Id", "INTEGER", "UNIQUE", "PRIMARY KEY", "AUTOINCREMENT"},
                      {"CamID", "INTEGER"}, {"StepID", "INTEGER"}, {"Time", "DATETIME"},
                      {"Filename", "TEXT"}}}};

    inline static const QHash<DB_TBL, QStringList> DB_INDEXES = {
        {DB_TBL_IMG, {"CamID", "StepID", "Time"}}};

signals:

public slots:
};

extern database *globalDB;

#endif // DATABASE_H
