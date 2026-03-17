QT       += core gui
QT       += openglwidgets
QT       += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ColoredMesh.cpp \
    ColoredModel.cpp \
    adaptivemeshsetting.cpp \
    dirshow.cpp \
    drawmodel.cpp \
    farfield3dwindow.cpp \
    farfieldobvpoint.cpp \
    fielddiagramshow.cpp \
    global.cpp \
    inputdlg.cpp \
    main.cpp \
    mainwindow.cpp \
    materialparameter.cpp \
    mesh.cpp \
    meshsettingsdialog.cpp \
    meshshow.cpp \
    meshtrans.cpp \
    model.cpp \
    mpisettingdlg.cpp \
    openglwidget.cpp \
    simulationdisp.cpp \
    simulationdlg.cpp \
    simulationmeshstruc.cpp \
    simulationmnt.cpp \
    simulationmonitor.cpp \
    simulationprobe.cpp \
    simulationressultset.cpp \
    simulationsetup.cpp \
    spshow.cpp\
    adaptivemeshsetting.cpp \
    unitssettingdialog.cpp

HEADERS += \
    Camera.h \
    ColoredMesh.h \
    ColoredModel.h \
    ColoredVertex.h \
    adaptivemeshsetting.h \
    dirshow.h \
    drawmodel.h \
    farfield3dwindow.h \
    farfieldobvpoint.h \
    fielddiagramshow.h \
    global.h \
    inputdlg.h \
    mainwindow.h \
    materialparameter.h \
    mesh.h \
    meshsettingsdialog.h \
    meshshow.h \
    meshtrans.h \
    model.h \
    mpisettingdlg.h \
    openglwidget.h \
    simulationdisp.h \
    simulationdlg.h \
    simulationmeshstruc.h \
    simulationmnt.h \
    simulationmonitor.h \
    simulationprobe.h \
    simulationresultset.h \
    simulationsetup.h \
    spshow.h\
    adaptivemeshsetting.h \
    unitssettingdialog.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lassimp-vc143-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -lassimp-vc143-mtd

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lassimp-vc143-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -lassimp-vc143-mtd

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lassimp-vc143-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -lassimp-vc143-mtd

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include
