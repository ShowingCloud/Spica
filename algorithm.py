# This Python file uses the following encoding: utf-8
from PySide6 import QtCore, QtNetwork, QtWidgets
import sys, json, signal


class algorithm(QtCore.QObject):
    def __init__(self, num):
        QtCore.QObject.__init__(self)

        self.memory = QtCore.QSharedMemory("algo_" + num, self)
        if (not self.memory.attach(QtCore.QSharedMemory.ReadOnly)):
            print('QSharedMemory: ', self.memory.error(), self.memory.errorString())

        self.socket = QtNetwork.QLocalSocket(self)
        self.socket.readyRead.connect(self.gotFrame)
        self.socket.errorOccurred.connect(
            lambda err: print('QLocalSocket Error Occurred: ', err))

        timer = QtCore.QTimer(self)
        timer.timeout.connect(
            lambda: self.socket.state() == QtNetwork.QLocalSocket.ConnectedState
                or self.socket.connectToServer("algo-" + num, QtCore.QIODevice.ReadWrite))
        timer.timeout.connect(
            lambda: self.memory.isAttached()
                or self.memory.attach(QtCore.QSharedMemory.ReadOnly)
                or print('QSharedMemory: ', self.memory.error(), self.memory.errorString()))
        QtCore.QTimer.singleShot(0, timer.timeout)
        timer.start(1000)

    def readMem(self, params):
        if (not self.memory.isAttached()):
            return QtCore.QByteArray("")

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
    algo = algorithm(sys.argv[1])
    signal.signal(signal.SIGINT, lambda signum, frame: app.exit())
    sys.exit(app.exec())
