#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#include "devplc.h"
#include "pylon.h"
#include "database.h"
#include "frontend.h"

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

    globalDB = new database();
    pylon::initialize();

    qmlRegisterType<devPLC>("spica.devplc", 1, 0, "DevPLC");
    qmlRegisterType<devPLCServer>("spica.devplc", 1, 0, "DevPLCServer");

    qmlRegisterSingletonType<frontend>("spica.frontend", 1, 0, "Frontend", [&](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return new frontend(&app);
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

    return app.exec();
}
