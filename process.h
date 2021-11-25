#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QTimer>
#include <iso646.h>

#include "devplc.h"
#include "pylon.h"
#include "database.h"

class product : public QObject
{
    Q_OBJECT

public:
    enum POS_LCR { POS_LEFT = 0, POS_CENTER, POS_RIGHT, POS_OTHERS };

    explicit product(const int id, const POS_LCR lcr, QObject *parent = nullptr) : QObject(parent), prodId(id), lcr(lcr) {
        for (const pylon::CAM_POS campos : pylon::camposList)
            algoReady[campos] = false;
    }

    friend database &operator<< (database &db, const product &prod);

    inline static product *find(const int id) {
        for (product *p : qAsConst(productList))
            if (p->prodId == id)
                return p;
        return nullptr;
    }

    inline static product *findOrCreate(const int id, const POS_LCR lcr, QObject *parent = nullptr) {
        product *ret = product::find(id);
        if (not ret) {
            ret = new product(id, lcr, parent);
            productList << ret;
        }
        return ret;
    }

    inline void setPneu(const int result) {
        pneuResult = result;
        pneuReady = true;
    }

    inline void setAlgo(const pylon::CAM_POS campos, const int setResult,
                        const int setImgId, const int setAlgoId) {
        imgId[campos] = setImgId;
        algoId[campos] = setAlgoId;
        algoResult[campos] = setResult;
        algoReady[campos] = true;
    }

    inline bool isPneuReady() {
        return pneuReady;
    }

    inline bool isAlgoReady(const pylon::CAM_POS campos) {
        return algoReady[campos];
    }

    inline bool isReady() {
        if (not pneuReady) return false;
        for (const pylon::CAM_POS campos : pylon::camposList)
            if (not algoReady[campos]) return false;

        result = 1;
        return true;
    }

    inline int getProdId() {
        return prodId;
    }

    inline static QVector<product *> productList = {};

private:
    int prodId;
    QHash<pylon::CAM_POS, bool> algoReady;
    QHash<pylon::CAM_POS, int> algoResult;
    QHash<pylon::CAM_POS, int> algoId;
    QHash<pylon::CAM_POS, int> imgId;
    bool pneuReady = false;
    int pneuResult;
    int result;
    POS_LCR lcr;
};
Q_DECLARE_METATYPE(product::POS_LCR)

class process : public QObject
{
    Q_OBJECT

public:
    explicit process(devPLC *dev, QObject *parent = nullptr) : QObject(parent), dev(dev), parent(parent) {
        for (devPLC::CAM_POS campos : devPLC::camposList) {
            for (int i = 0; i < 3; ++i) {
                camProdId[i][campos] = 0;
                camProd[i][campos] = nullptr;
            }
            camReady[campos] = false;
        }
        for (pylon::CAM_POS campos : pylon::camposList) {
            imgId[campos] = 0;
            algoId[campos] = 0;
            algoResult[campos] = {0, 0, 0};
            algoReady[campos] = false;
        }
    }

    static void startProcessing(devPLC *dev, QObject *parent = nullptr);
#ifdef QT_DEBUG
    static void startServer(devPLCServer *dev, QObject *parent = nullptr);
#endif
    void processing();

    inline static const int checkInterval = 500;
    inline static const int camReadyTimeout = 4000;
    inline static const int algoTimeout = 4000;
    inline static const int readPlcTimeout = 1000;
    inline static const int processTimeout = 6500;

signals:

private:
    devPLC *dev;
    QObject *parent;
    bool gotProd = false;
    int pneuProdId[3] = {0, 0, 0};
    product *pneuProd[3] = {nullptr, nullptr, nullptr};
    int pneuResult[3] = {0, 0, 0};
    bool pneuReady = false;
    QHash<devPLC::CAM_POS, int> camProdId[3];
    QHash<devPLC::CAM_POS, product *> camProd[3];
    QHash<devPLC::CAM_POS, bool> camReady;
    QHash<pylon::CAM_POS, int> imgId;
    QHash<pylon::CAM_POS, int> algoId;
    QHash<pylon::CAM_POS, QVector<int>> algoResult;
    QHash<pylon::CAM_POS, bool> algoReady;
};

#endif // PROCESS_H
