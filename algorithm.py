# This Python file uses the following encoding: utf-8
from PySide6 import QtCore


class algorithm(QtCore.QObject):
    def __init__(self):
        QtCore.QObject.__init__(self)

    def mem(self):
        memory = QtCore.QSharedMemory("algo_1", self)
        memory.attach(QtCore.QSharedMemory.ReadOnly)
        memory.lock()
        QtCore.qDebug('%s' % QtCore.QCryptographicHash.hash(
            QtCore.QByteArray(memory.constData().tobytes()
            ), QtCore.QCryptographicHash.Md5).toHex())
        memory.unlock()

if __name__ == '__main__':
    algo = algorithm()
    algo.mem()
