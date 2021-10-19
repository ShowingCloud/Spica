#include "frontend.h"
#include "database.h"

#include <QFileInfo>

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
