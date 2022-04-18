##
## 说明：
## 本子模块(api.pri)用于：用户针对easyeai_api的拓展封装，或者自定义补充封装
##

## Qt工程  针对easyapi_api目录与库的引用
## ========================== easyeai_api ========================== ##
INCLUDEPATH += \
    ../easyeai-api/common_api/system_opt \
    ../easyeai-api/peripheral_api/display \
    ../easyeai-api/peripheral_api/camera \

LIBS += $$PWD/../../../easyeai-api/common_api/system_opt/*.a
LIBS += $$PWD/../../../easyeai-api/peripheral_api/display/*.a  -ldrm
LIBS += $$PWD/../../../easyeai-api/peripheral_api/camera/*.a -leasymedia -lrga -lrkfacial -lrkisp_api
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
    $$PWD/Camera/Camera.h \
    $$PWD/Display/Display.h \
## ================================================================= ##
