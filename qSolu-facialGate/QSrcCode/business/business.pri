##
## 说明：
## 本子模块(business.pri)用作：管理和实现业务逻辑
##
INCLUDEPATH += \
    $$PWD/ \

SOURCES += \
    $$PWD/appscheduler.cpp \
    $$PWD/basemodel.cpp \
    $$PWD/dataBase/dataBaseMgr.cpp \
    $$PWD/mainThread/mainThread.cpp \
    $$PWD/msgAdapter/msgAdapter.cpp \
    $$PWD/uiManager/uimanager.cpp \
#    $$PWD/algorithmCtrl/algorithmCtrl.cpp \
#    $$PWD/doorContrl/doorcontrol.cpp \
    $$PWD/Announcement/announcement.cpp \

HEADERS += \
    $$PWD/appscheduler.h \
    $$PWD/bsprotocol.h \
    $$PWD/basemodel.h \
    $$PWD/dataBase/dataBaseMgr.h \
    $$PWD/mainThread/mainThread.h \
    $$PWD/msgAdapter/msgAdapter.h \
    $$PWD/uiManager/uimanager.h \
#    $$PWD/algorithmCtrl/algorithmCtrl.h \
#    $$PWD/doorContrl/doorcontrol.h \
    $$PWD/Announcement/announcement.h \
