#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#include "devplc.h"
#include "pylon.h"
#include "database.h"
#include "frontend.h"
#include "process.h"

database *globalDB;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

#if 0
#ifndef QT_DEBUG
    qInstallMessageHandler(
                [](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        Q_UNUSED(type) Q_UNUSED(context) Q_UNUSED(msg)
    });
#endif
#endif

    QTranslator translator;
    translator.load(":/i18n/zh_CN");
    app.installTranslator(&translator);

    globalDB = new database(&app);
    pylon::initialize(&app);
    devPLC plc(&app);
    process::startProcessing(&plc, &app);
#ifdef QT_DEBUG
    devPLCServer plcserver(&app);
    process::startServer(&plcserver, &app);
#endif

    qmlRegisterSingletonType<frontend>("spica.frontend", 1, 0, "Frontend", [&app](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new frontend(&app);
    });
    qmlRegisterSingletonType<productRecordModel>("spica.frontend", 1, 0, "ProductRecordModel", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new productRecordModel();
    });

    QQmlApplicationEngine engine;
#ifdef QT_DEBUG
    engine.rootContext()->setContextProperty("debug", true);
#else
    engine.rootContext()->setContextProperty("debug", false);
#endif
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    int ret = app.exec();
    pylon::destroy();
    plc.deleteLater();
#ifdef QT_DEBUG
    plcserver.deleteLater();
#endif
    delete globalDB;
    return ret;
}
