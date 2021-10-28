# This Python file uses the following encoding: utf-8
from PySide6 import QtCore, QtNetwork, QtWidgets
import sys, json


class algorithm(QtCore.QObject):
    def __init__(self):
        QtCore.QObject.__init__(self)

        self.memory = QtCore.QSharedMemory("algo_1", self)
        if (not self.memory.attach(QtCore.QSharedMemory.ReadOnly)):
            print('QSharedMemory: ', self.memory.error(), self.memory.errorString())

        self.socket = QtNetwork.QLocalSocket(self)
        self.socket.connected.connect(
            lambda: self.socket.readyRead.connect(self.gotFrame))
        self.socket.errorOccurred.connect(
            lambda err: print('QLocalSocket Error Occurred: ', err))
        self.socket.connectToServer("algo-1", QtCore.QIODevice.ReadWrite)

    def readMem(self, params):
        self.memory.lock()
        QtCore.qDebug('%s' % QtCore.QCryptographicHash.hash(
            QtCore.QByteArray(self.memory.constData().tobytes()).left(params["size"]),
            QtCore.QCryptographicHash.Md5).toHex())
        self.memory.unlock()

        return QtCore.QByteArray(json.dumps({
            "id": params["id"],
            "size": params["size"]
            }))

    def gotFrame(self):
        req = self.socket.readAll()
        params = json.loads(req.data())
        self.socket.write(self.readMem(params))

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    algo = algorithm()
    sys.exit(app.exec())
