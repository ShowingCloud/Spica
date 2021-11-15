#include <QDateTime>

#include "process.h"

void process::startProcessing(devPLC *dev, QObject *parent)
{
    QTimer *stateTimer = new QTimer(parent);
    QObject::connect(stateTimer, &QTimer::timeout, [=]() {
        if (not dev->readData(dev->addrState, 1, [=](QVector<quint16> resp) {

            if (resp.length() != 1 or resp.value(0) != 1) {
                stateTimer->start(500);
                return;
            }

            process *proc = new process(dev, parent);
            proc->processing();
            stateTimer->deleteLater();
        }))
            stateTimer->start(500);
    });
    stateTimer->start(0);
}

#ifdef QT_DEBUG
void process::startServer(devPLCServer *dev, QObject *parent)
{
    QTimer *timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [=]() {
        static int i = 0;
        switch(i) {
        case 0:
            dev->dev->setData(QModbusDataUnit::HoldingRegisters, 100, 1);
            break;
        case 1:
            break;
        default:
            qDebug() << i;
            ++i;
            break;
        }
    });
    timer->start(1000);
}
#endif

void process::processing()
{
    QTimer *prodTimer = new QTimer(this);
    connect(prodTimer, &QTimer::timeout, [=]() {
        prodTimer->setInterval(500);
        int startAddr = dev->camProdAddr[devPLC::CAM_POS_B][0];
        if (not dev->readData(startAddr, dev->prodAddrLen, [=](QVector<quint16> resp) {
            if (resp.length() != dev->prodAddrLen) {
                prodTimer->start(500);
                return;
            }

            for (int i = 0; i < 3; ++i) {
                pneuProdId[i] = resp[dev->pneuProdAddr[i] - startAddr];
                pneuProd[i] = product::findOrCreate(pneuProdId[i], this);
            }

            for (const devPLC::CAM_POS campos : devPLC::camposList)
                for (int i = 0; i < 3; ++i) {
                    camProdId[i][campos] = resp[dev->camProdAddr[campos][i] - startAddr];
                    camProd[i][campos] = product::findOrCreate(camProdId[i][campos], this);
                }

            gotProd = true;
            prodTimer->deleteLater();
        }))
            prodTimer->start(500);
    });
    prodTimer->start(0);

    QTimer *pneuTimer = new QTimer(this);
    connect(pneuTimer, &QTimer::timeout, [=]() {
        pneuTimer->setInterval(500);
        if (not dev->readData(dev->pneuAddr[0], dev->pneuAddrLen, [=](QVector<quint16> resp) {
            if (resp.length() != dev->pneuAddrLen) {
                pneuTimer->start(500);
                return;
            }

            for (int i = 0; i < 3; ++i) {
                pneuResult[i] = resp[dev->pneuAddr[i] - dev->pneuAddr[0]];
                if (gotProd)
                    pneuProd[i]->setPneu(pneuResult[i]);
            }

            pneuReady = true;
            pneuTimer->deleteLater();
        }))
            pneuTimer->start(500);
    });
    pneuTimer->start(0);

    QTimer *camReadyTimer = new QTimer(this);
    connect(camReadyTimer, &QTimer::timeout, [=]() {
        camReadyTimer->setInterval(500);
        int startAddr = dev->camReadAddr[devPLC::CAM_POS_B];
        if (not dev->readData(startAddr, dev->camReadAddrLen, [=](QVector<quint16> resp) {
            if (resp.length() != dev->camReadAddrLen) {
                camReadyTimer->start(500);
                return;
            }

            for (const devPLC::CAM_POS campos : devPLC::camposList) {
                if (not camReady[campos]) {
                    if (resp[dev->camReadAddr[campos] - startAddr] == 1) {
                        for (const pylon::CAM_POS pos : pylon::positionStation.keys(campos)) {
                            imgId[pos] = pylon::posDevList[pos]->capture(); // TODO: might get -1

                            QMetaObject::Connection * const conn = new QMetaObject::Connection;
                            *conn = connect(pylon::posDevList[pos], &pylon::gotAlgo,
                                [=](const int gotImgId, const int gotAlgoId, const QVector<int> gotResult) {
                                    if (gotImgId != imgId[pos])
                                        qDebug() << "!!! Should be equal: imgId: " << gotImgId << imgId[pos];

                                    algoId[pos] = gotAlgoId;
                                    algoResult[pos] = gotResult;
                                    algoReady[pos] = true;

                                    if (gotProd) {
                                        for (int i = 0; i < 3; ++i) {
                                            product *prod = camProd[i][campos];
                                            prod->setAlgo(pos, gotResult[i], gotImgId, gotAlgoId);
                                        }
                                    }

                                    disconnect(*conn);
                                    delete conn;
                            });
                        }

                        camReady[campos] = true;
                    } else {
                        camReadyTimer->start(500);
                        return;
                    }
                }
            }
            camReadyTimer->deleteLater();
        }))
            camReadyTimer->start(500);
    });
    camReadyTimer->start(0);

    QTimer *waitTimer = new QTimer(this);
    connect(waitTimer, &QTimer::timeout, [=]() {
        waitTimer->setInterval(500);
        if (not gotProd) {
            waitTimer->start(500);
            return;
        }

        for (const pylon::CAM_POS campos : pylon::camposList) {
            if (not algoReady[campos]) {
                waitTimer->start(500);
                return;
            }

            for (int i = 0; i < 3; ++i) {
                product *prod = camProd[i][pylon::positionStation[campos]];
                if (not prod->isAlgoReady(campos))
                    prod->setAlgo(campos, algoResult[campos][i], imgId[campos], algoId[campos]);

                if (prod->isReady()) {
                    *globalDB << *prod;
                    qDebug() << "Removed from product list: " << product::productList.removeAll(prod);
                }
            }
        }

        if (not pneuReady) {
            waitTimer->start(500);
            return;
        }
        for (int i = 0; i < 3; ++i) {
            product *prod = pneuProd[i];
            if (not prod->isPneuReady())
                prod->setPneu(pneuResult[i]);

            if (prod->isReady()) {
                *globalDB << *prod;
                qDebug() << "Removed from product list: " << product::productList.removeAll(prod);
            }
        }

        waitTimer->deleteLater();
    });
    waitTimer->start(0);

    QVector<quint16> writeCam = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    if (not dev->writeData(dev->camWriteAddr[devPLC::CAM_POS_B][0], writeCam, [](bool ret){
            if (not ret) qDebug() << "Write camera results error";
        }))
        qDebug() << "Write camera results error";
}

database &operator<< (database &db, const product &prod)
{
    db.dbModel->setTable(db.DB_TABLES[db.DB_TBL_PROD]);
    QSqlRecord r = db.dbModel->record();
    r.setValue("ProdID", prod.prodId);
    r.setValue("PneuResult", prod.pneuResult);
    r.setValue("Time", QDateTime::currentDateTime());
    for (const pylon::CAM_POS campos : pylon::camposList) {
        r.setValue("Cam" + QString::number(campos) + "Img", prod.imgId[campos]);
        r.setValue("Cam" + QString::number(campos) + "Algo", prod.algoId[campos]);
        r.setValue("Cam" + QString::number(campos) + "Result", prod.algoResult[campos]);
    }
    if (!db.dbModel->insertRecord(-1, r))
        qDebug() << db.dbModel->lastError();

    return db;
}
