# This Python file uses the following encoding: utf-8
from PySide6 import QtCore, QtNetwork, QtWidgets
import sys


class algorithm(QtCore.QObject):
    def __init__(self):
        QtCore.QObject.__init__(self)

        self.memory = QtCore.QSharedMemory("algo_1", self)
        self.memory.attach(QtCore.QSharedMemory.ReadOnly)

        self.socket = QtNetwork.QLocalSocket(self)
        self.socket.connected.connect(
            lambda: self.socket.readyRead.connect(self.gotFrame))
        self.socket.errorOccurred.connect(
            lambda err: QtCore.qDebug('%d' % err))
        self.socket.connectToServer("algo-1", QtCore.QIODevice.ReadWrite)

    def readMem(self):
        self.memory.lock()
        QtCore.qDebug('%s' % QtCore.QCryptographicHash.hash(
            QtCore.QByteArray(self.memory.constData().tobytes()).left(1920000),
            QtCore.QCryptographicHash.Md5).toHex())
        self.memory.unlock()
        return QtCore.QByteArray("return success")

    def gotFrame(self):
        data = self.socket.readAll()
        QtCore.qDebug('%s' % data)
        self.socket.write(self.readMem())

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    algo = algorithm()
    sys.exit(app.exec())
