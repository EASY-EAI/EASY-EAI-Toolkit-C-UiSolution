##
## 说明：
## 本子模块(common.pri)用作：用户对第三方模块的管理(库或者源代码)
##

INCLUDEPATH += \
    $$PWD/

SOURCES += \
    $$PWD/dataBase/dataBase.cpp \
    $$PWD/wavPlayer/wavSplicer.cpp \
    $$PWD/wavPlayer/play_wav.cpp \

HEADERS += \
    $$PWD/system.h \
    $$PWD/dataBase/dataBase.h \
    $$PWD/wavPlayer/wavSplicer.h \
    $$PWD/wavPlayer/play_wav.h \
