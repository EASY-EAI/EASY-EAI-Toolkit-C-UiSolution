##
## 说明：
## 本子模块(api.pri)用于：用户针对easyeai_api的拓展封装，或者自定义补充封装
##

## Qt工程  针对easyapi_api目录与库的引用
## ========================== easyeai_api ========================== ##
INCLUDEPATH += \
    ../easyeai-api/common_api/system_opt \
    ../easyeai-api/peripheral_api/network \
    ../easyeai-api/peripheral_api/display \
    ../easyeai-api/peripheral_api/camera \

LIBS += -L$$PWD/../../../easyeai-api/common_api/system_opt/          -lsystem_opt -pthread
LIBS += -L$$PWD/../../../easyeai-api/peripheral_api/network/         -lnetwork
LIBS += -L$$PWD/../../../easyeai-api/peripheral_api/display/         -ldisplay -leasymedia
LIBS += -L$$PWD/../../../easyeai-api/peripheral_api/camera/          -lcamera  -leasymedia -lrga -lrkfacial -lrkisp_api -lrkaiq
## ================================================================= ##



## Qt工程  针对用户自定义的拓展模块文件管理
## (模块命名建议：使用easyeai_api库对应的驼峰命名法。如：system_opt -> SystemOpt)
## ============================= custom ============================ ##
INCLUDEPATH += \
    $$PWD/ \

SOURCES += \
    $$PWD/SystemOpt/SystemOpt.cpp \

HEADERS += \
    $$PWD/SystemOpt/SystemOpt.h \
    $$PWD/Network/Network.h \
    $$PWD/Display/Display.h \
    $$PWD/Camera/Camera.h \
## ================================================================= ##
