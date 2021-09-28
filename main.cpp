#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#include "devplc.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QTranslator translator;
    translator.load(":/i18n/zh_CN");
    app.installTranslator(&translator);

    qmlRegisterType<devPLC>("spica.devplc", 1, 0, "DevPLC");
    qmlRegisterType<devPLCServer>("spica.devplc", 1, 0, "DevPLCServer");

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
