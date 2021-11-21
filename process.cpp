#include <QDateTime>

#include "process.h"

void process::startProcessing(devPLC *dev, QObject *parent)
{
    QTimer *stateTimer = new QTimer(parent);
    QObject::connect(stateTimer, &QTimer::timeout, [=]() {
        if (not dev->readData(dev->addrState, 1, [=](QVector<quint16> resp) {

            if (resp.length() != 1 or resp.value(0) != 1) {
                stateTimer->start(checkInterval);
                return;
            }

            process *proc = new process(dev, parent);
            proc->processing();
            stateTimer->deleteLater();
        }))
            stateTimer->start(checkInterval);
    });
    stateTimer->start(0);
}

#ifdef QT_DEBUG
void process::startServer(devPLCServer *dev, QObject *parent)
{
    QTimer *timer = new QTimer(parent);
    QObject::connect(timer, &QTimer::timeout, [=]() {
        static int i = 0;
        QHash<quint16, quint16> data;
        switch(i) {
        case 0:
            data = {
                {100, 1}, // 1200
                {2, 5}, {4, 6}, {6, 7}, {8, 9}, //1号模位编号
                {12, 13}, {14, 15}, {16, 17}, {18, 19}, //2号模位编号
                {22, 23}, {24, 25}, {26, 27}, {28, 29}, //3号模位编号
                {31, 0}, {32, 1}, {33, 1}, {34, 1}, //1号气动结果
                {36, 1}, {37, 1}, {38, 1}, {39, 1}, //2号气动结果
                {41, 1}, {42, 1}, {43, 1}, {44, 1}, //3号气动结果
            };
            break;
        case 1:
            data = {{66, 1}}; // B拍照ready
            break;
        case 2:
            data = {{67, 1}};// C拍照ready
            break;
        case 3:
            data = {{68, 1}};// D拍照ready
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            break;
        case 8:
            data = {
                {100, 1}, // 1200
                {2, 30}, {4, 5}, {6, 6}, {8, 7}, //1号模位编号
                {12, 31}, {14, 13}, {16, 15}, {18, 17}, //2号模位编号
                {22, 32}, {24, 23}, {26, 25}, {28, 27}, //3号模位编号
                {31, 1}, {31, 0}, {32, 1}, {33, 1}, //1号气动结果
                {36, 0}, {37, 1}, {38, 1}, {39, 1}, //2号气动结果
                {41, 1}, {42, 1}, {43, 1}, {44, 1}, //3号气动结果
            };
            break;
        case 9:
            data = {{66, 1}}; // B拍照ready
            break;
        case 10:
            data = {{67, 1}};// C拍照ready
            break;
        case 11:
            data = {{68, 1}};// D拍照ready
            break;
        case 12:
        case 13:
        case 14:
        case 15:
            break;
        case 16:
            data = {
                {100, 1}, // 1200
                {2, 33}, {4, 30}, {6, 5}, {8, 6}, //1号模位编号
                {12, 34}, {14, 31}, {16, 13}, {18, 15}, //2号模位编号
                {22, 35}, {24, 32}, {26, 23}, {28, 25}, //3号模位编号
                {31, 1}, {31, 1}, {32, 0}, {33, 1}, //1号气动结果
                {36, 1}, {37, 0}, {38, 1}, {39, 1}, //2号气动结果
                {41, 0}, {42, 1}, {43, 1}, {44, 1}, //3号气动结果
            };
            break;
        case 17:
            data = {{66, 1}}; // B拍照ready
            break;
        case 18:
            data = {{67, 1}};// C拍照ready
            break;
        case 19:
            data = {{68, 1}};// D拍照ready
            break;
        case 20:
        case 21:
        case 22:
        case 23:
            break;
        case 24:
            data = {
                {100, 1}, // 1200
                {2, 36}, {4, 33}, {6, 30}, {8, 5}, //1号模位编号
                {12, 37}, {14, 34}, {16, 31}, {18, 13}, //2号模位编号
                {22, 38}, {24, 35}, {26, 32}, {28, 23}, //3号模位编号
                {31, 1}, {31, 1}, {32, 1}, {33, 0}, //1号气动结果
                {36, 1}, {37, 1}, {38, 0}, {39, 1}, //2号气动结果
                {41, 1}, {42, 0}, {43, 1}, {44, 1}, //3号气动结果
            };
            break;
        case 25:
            data = {{66, 1}}; // B拍照ready
            break;
        case 26:
            data = {{67, 1}};// C拍照ready
            break;
        case 27:
            data = {{68, 1}};// D拍照ready
            break;
        case 28:
        case 29:
        case 30:
        case 31:
            break;
        case 32:
            data = {
                {100, 1}, // 1200
                {2, 39}, {4, 36}, {6, 33}, {8, 30}, //1号模位编号
                {12, 40}, {14, 37}, {16, 34}, {18, 31}, //2号模位编号
                {22, 41}, {24, 38}, {26, 35}, {28, 32}, //3号模位编号
                {31, 1}, {31, 1}, {32, 1}, {33, 1}, //1号气动结果
                {36, 1}, {37, 1}, {38, 1}, {39, 0}, //2号气动结果
                {41, 1}, {42, 1}, {43, 0}, {44, 1}, //3号气动结果
            };
            break;
        case 33:
            data = {{66, 1}}; // B拍照ready
            break;
        case 34:
            data = {{67, 1}};// C拍照ready
            break;
        case 35:
            data = {{68, 1}};// D拍照ready
            break;
        case 36:
        case 37:
        case 38:
        case 39:
            break;
        default:
            break;
        }

        for (const quint16 &key : data.keys())
            dev->dev->setData(QModbusDataUnit::HoldingRegisters, key, data[key]);

        ++i;
    });
    timer->start(1000);
}
#endif

void process::processing()
{
    QDateTime prodTime = QDateTime::currentDateTime();
    QTimer *prodTimer = new QTimer(this);
    connect(prodTimer, &QTimer::timeout, [=]() {
        prodTimer->setInterval(checkInterval);
        int startAddr = dev->camProdAddr[devPLC::CAM_POS_B][0];
        if (not dev->readData(startAddr, dev->prodAddrLen, [=](QVector<quint16> resp) {

            if (resp.length() != dev->prodAddrLen) {
                if (prodTime.msecsTo(QDateTime::currentDateTime()) > readPlcTimeout) {
                    prodTimer->deleteLater();
                    return;
                }

                prodTimer->start(checkInterval);
                return;
            }

            for (int i = 0; i < 3; ++i) {
                pneuProdId[i] = resp[dev->pneuProdAddr[i] - startAddr];
                pneuProd[i] = product::findOrCreate(
                                  pneuProdId[i], static_cast<product::POS_LCR>(i), this);
            }

            for (const devPLC::CAM_POS campos : devPLC::camposList)
                for (int i = 0; i < 3; ++i) {
                    camProdId[i][campos] = resp[dev->camProdAddr[campos][i] - startAddr];
                    camProd[i][campos] = product::findOrCreate(
                                  camProdId[i][campos], static_cast<product::POS_LCR>(i), this);
                }

            gotProd = true;
            prodTimer->deleteLater();
        }))
            prodTimer->start(checkInterval);
    });
    prodTimer->start(0);

    QDateTime pneuTime = QDateTime::currentDateTime();
    QTimer *pneuTimer = new QTimer(this);
    connect(pneuTimer, &QTimer::timeout, [=]() {
        pneuTimer->setInterval(checkInterval);
        if (not dev->readData(dev->pneuAddr[0], dev->pneuAddrLen, [=](QVector<quint16> resp) {

            if (resp.length() != dev->pneuAddrLen) {
                if (pneuTime.msecsTo(QDateTime::currentDateTime()) > readPlcTimeout) {
                    pneuTimer->deleteLater();
                    return;
                }

                pneuTimer->start(checkInterval);
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
            pneuTimer->start(checkInterval);
    });
    pneuTimer->start(0);

    QDateTime camReadyTime = QDateTime::currentDateTime();
    QTimer *camReadyTimer = new QTimer(this);
    connect(camReadyTimer, &QTimer::timeout, [=]() {
        int startAddr = dev->camReadAddr[devPLC::CAM_POS_B];
        if (not dev->readData(startAddr, dev->camReadAddrLen, [=](QVector<quint16> resp) {

            if (resp.length() != dev->camReadAddrLen) {
                if (camReadyTime.msecsTo(QDateTime::currentDateTime()) > camReadyTimeout) {
                    camReadyTimer->deleteLater();
                    return;
                }

                camReadyTimer->start(checkInterval);
                return;
            }

            for (const devPLC::CAM_POS campos : devPLC::camposList)

                if (not camReady[campos]) {
                    if (resp[dev->camReadAddr[campos] - startAddr] == 1) {
                        for (const pylon::CAM_POS pos : pylon::positionStation.keys(campos)) {
                            imgId[pos] = pylon::posDevList[pos]->capture(); // TODO: might get -1

                            QDateTime startCamTime = QDateTime::currentDateTime();
                            QMetaObject::Connection * const conn = new QMetaObject::Connection;
                            *conn = connect(pylon::posDevList[pos], &pylon::gotAlgo,
                                [=](const int gotImgId, const int gotAlgoId, const QVector<int> gotResult) {
                                if (gotImgId != imgId[pos])
                                    qDebug() << "!!! Should be equal: imgId: " << gotImgId << imgId[pos];

                                algoId[pos] = gotAlgoId;
                                algoResult[pos] = gotResult;
                                algoReady[pos] = true;
                                qWarning() << "Got algo reply" << imgId[pos] << algoReady
                                            << startCamTime.msecsTo(QDateTime::currentDateTime());

                                if (gotProd)
                                    for (int i = 0; i < 3; ++i) {
                                        product *prod = camProd[i][campos];
                                        prod->setAlgo(pos, gotResult[i], gotImgId, gotAlgoId);
                                    }

                                if (conn)
                                    disconnect(*conn);
                            });

                            QTimer *algoTimeoutTimer = new QTimer(this);
                            connect(algoTimeoutTimer, &QTimer::timeout, [=]() {
                                if (conn) {
                                    disconnect(*conn);
                                    delete conn;
                                }
                                algoTimeoutTimer->deleteLater();
                            });
                            algoTimeoutTimer->start(algoTimeout);
                        }

                        camReady[campos] = true;
                    } else {
                        camReadyTimer->start(checkInterval);
                        return;
                    }
                }

            camReadyTimer->deleteLater();
        }))
            camReadyTimer->start(checkInterval);
    });
    camReadyTimer->start(checkInterval); // check only after moments after for light to be prepared

    QDateTime waitTime = QDateTime::currentDateTime();
    QTimer *waitTimer = new QTimer(this);
    connect(waitTimer, &QTimer::timeout, [=]() {
        if (not gotProd) {
            if (waitTime.msecsTo(QDateTime::currentDateTime()) > processTimeout) {
                waitTimer->deleteLater();
                process::startProcessing(dev, parent);
                return;
            }
            waitTimer->start(checkInterval);
            return;
        }

        bool timedout = (waitTime.msecsTo(QDateTime::currentDateTime()) > processTimeout);

        for (const pylon::CAM_POS campos : pylon::camposList) {
            if (not timedout and not algoReady[campos]) {
                waitTimer->start(checkInterval);
                return;
            }

            for (int i = 0; i < 3; ++i) {
                product *prod = camProd[i][pylon::positionStation[campos]];
                if (timedout and not algoReady[campos]) {
                    qDebug() << "Timeout on algo ready" << prod->getProdId() << algoReady << waitTime.msecsTo(QDateTime::currentDateTime());
                    prod->setAlgo(campos, false, 0, 0);
                }
                else if (not prod->isAlgoReady(campos)) {
                    qInfo() << "Setting algo ready" << prod->getProdId() << algoReady;
                    prod->setAlgo(campos, algoResult[campos][i], imgId[campos], algoId[campos]);
                }
            }
        }
        // We're here either because all algos ready or when processTimeout

        for (const devPLC::CAM_POS campos : devPLC::camposList) {
            for (int i = 0; i < 3 ; ++i) {
                bool result = true;
                for (const pylon::CAM_POS cam : pylon::positionStation.keys(campos))
                    result = (algoResult[cam][i] == 1);

                if (not dev->writeData(dev->camWriteAddr[campos][i], {result}, [](bool ret) {
                    if (not ret) qDebug() << "Write camera result error";
                }))
                    qDebug() << "Write camera result error";
            }
        }

        if (not pneuReady and not timedout) {
            waitTimer->start(checkInterval);
            return;
        }
        for (int i = 0; i < 3; ++i) {
            product *prod = pneuProd[i];
            if (not prod->isPneuReady())
                prod->setPneu(pneuResult[i]);
            else if (timedout)
                prod->setPneu(false);

            if (prod->isReady()) { // Both pneu and algo ready are set above when timeout
                *globalDB << *prod;
                qDebug() << "Removed from product list: " << prod->getProdId() << product::productList.removeAll(prod);
            }
        }

        qDebug() << "Finished one round of processing, resetting state";
        if (not dev->writeData(dev->addrState, {2}, [](bool ret){
            if (not ret) qDebug() << "Write state error";
        }))
            qDebug() << "Write state error";

        waitTimer->deleteLater();

        process::startProcessing(dev, parent);
    });
    waitTimer->start(checkInterval); // check only after moments later
}

database &operator<< (database &db, const product &prod)
{
    db.dbModel->setTable(db.DB_TABLES[db.DB_TBL_PROD]);
    QSqlRecord r = db.dbModel->record();
    r.setValue("ProdID", prod.prodId);
    r.setValue("PneuResult", prod.pneuResult);
    r.setValue("Time", QDateTime::currentDateTime());
    r.setValue("LCR", prod.lcr);
    for (const pylon::CAM_POS campos : pylon::camposList) {
        r.setValue("Cam" + QString::number(campos) + "Img", prod.imgId[campos]);
        r.setValue("Cam" + QString::number(campos) + "Algo", prod.algoId[campos]);
        r.setValue("Cam" + QString::number(campos) + "Result", prod.algoResult[campos]);
    }
    if (!db.dbModel->insertRecord(-1, r))
        qDebug() << db.dbModel->lastError();

    return db;
}
