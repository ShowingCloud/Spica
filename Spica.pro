QT += quick sql serialport serialbus widgets
CONFIG += c++1z console lrelease embed_translations

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        algorithm.cpp \
        database.cpp \
        devplc.cpp \
        frontend.cpp \
        main.cpp \
        pylon.cpp

RESOURCES += qml.qrc

TRANSLATIONS += zh_CN.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    algorithm.h \
    database.h \
    devplc.h \
    frontend.h \
    pylon.h

CONFIG(release, debug|release) {
    DEFINES += QT_NO_WARNING_OUTPUT
}
DEFINES += QT_NO_INFO_OUTPUT

unix:!macx {
    CONFIG(debug, debug|release) {
        INCLUDEPATH += /opt/pylon/include
        LIBS += -L/opt/pylon/lib -Wl,-E -lpylonbase -lpylonutility -lGenApi_gcc_v3_1_Basler_pylon -lGCBase_gcc_v3_1_Basler_pylon
        QMAKE_LFLAGS += -Wl,--enable-new-dtags -Wl,-rpath,/opt/pylon/lib
    }
    CONFIG(release, debug|release) {
        INCLUDEPATH += ../Pylon/Linux/include
        LIBS += -L../Pylon/Linux/lib -lpylonbase -lpylonutility -lGenApi_gcc_v3_1_Basler_pylon -lGCBase_gcc_v3_1_Basler_pylon
    }
}
macx {
    CONFIG(debug, debug|release) {
        INCLUDEPATH += /Library/Frameworks/pylon.framework/Headers /Library/Frameworks/pylon.framework/Headers/GenICam
        LIBS += -L/Library/Frameworks/pylon.framework/Libraries -lpylonbase -lpylonutility -lGenApi_gcc_v3_1_Basler_pylon -lGCBase_gcc_v3_1_Basler_pylon
        QMAKE_LFLAGS += -Wl,-rpath,/Library/Frameworks/pylon.framework/Libraries
    }
    CONFIG(release, debug|release) {
        INCLUDEPATH += ../Pylon/MacOS/Headers ../Pylon/MacOS/Headers/GenICam
        LIBS += -L../Pylon/MacOS/Libraries -lpylonbase -lpylonutility -lGenApi_gcc_v3_1_Basler_pylon -lGCBase_gcc_v3_1_Basler_pylon
    }
}
win32 {
    CONFIG(release, debug|release) {
        INCLUDEPATH += ..\Pylon\Windows\include
        !contains(QMAKE_TARGET.arch, x86_64) {
            QMAKE_LFLAGS += -LIBPATH:..\Pylon\Windows\lib\Win32
        } else {
            QMAKE_LFLAGS += -LIBPATH:..\Pylon\Windows\lib\x64
        }
    }
    CONFIG(debug, debug|release) {
        INCLUDEPATH += $$(PYLON_DEV_DIR)\include
        !contains(QMAKE_TARGET.arch, x86_64) {
            QMAKE_LFLAGS += -LIBPATH:\"$$(PYLON_DEV_DIR)\lib\Win32\"
        } else {
            QMAKE_LFLAGS += -LIBPATH:\"$$(PYLON_DEV_DIR)\lib\x64\"
        }
    }
}

DISTFILES += \
    algorithm.py
