##
## 说明：
## 本子模块(api.pri)用于：用户针对easyeai_api的拓展封装，或者自定义补充封装
##

## Qt工程  针对easyapi_api目录与库的引用
## ========================== easyeai_api ========================== ##
INCLUDEPATH += \
    ../easyeai-api/algorithm_api/geometry \
    ../easyeai-api/algorithm_api/face_alignment \
    ../easyeai-api/algorithm_api/face_detect \
    ../easyeai-api/algorithm_api/face_recognition \
    ../easyeai-api/common_api/file_opt \
    ../easyeai-api/common_api/string_opt \
    ../easyeai-api/common_api/system_opt \
    ../easyeai-api/common_api/ini_wrapper \
    ../easyeai-api/peripheral_api/display \
    ../easyeai-api/peripheral_api/camera \

LIBS += -L$$PWD/../../../easyeai-api/algorithm_api/geometry/         -lgeometry
LIBS += -L$$PWD/../../../easyeai-api/algorithm_api/face_alignment/   -lface_alignment   -lrknn_api
LIBS += -L$$PWD/../../../easyeai-api/algorithm_api/face_detect/      -lface_detect      -lrknn_api
LIBS += -L$$PWD/../../../easyeai-api/algorithm_api/face_recognition/ -lface_recognition -lrknn_api
LIBS += -L$$PWD/../../../easyeai-api/common_api/file_opt/            -lfile_opt
LIBS += -L$$PWD/../../../easyeai-api/common_api/string_opt/          -lstring_opt
LIBS += -L$$PWD/../../../easyeai-api/common_api/system_opt/          -lsystem_opt -pthread
LIBS += -L$$PWD/../../../easyeai-api/common_api/ini_wrapper/         -lini_wrapper -lini
LIBS += -L$$PWD/../../../easyeai-api/peripheral_api/display/         -ldisplay -leasymedia
LIBS += -L$$PWD/../../../easyeai-api/peripheral_api/camera/          -lcamera  -leasymedia -lrga -lrkfacial -lrkisp_api
## ================================================================= ##



## Qt工程  针对用户自定义的拓展模块文件管理
## (模块命名建议：使用easyeai_api库对应的驼峰命名法。如：system_opt -> SystemOpt)
## ============================= custom ============================ ##
INCLUDEPATH += \
    $$PWD/ \

SOURCES += \
    $$PWD/FileOpt/FileOpt.cpp \
    $$PWD/SystemOpt/SystemOpt.cpp \

HEADERS += \
    $$PWD/Camera/Camera.h \
    $$PWD/Display/Display.h \
    $$PWD/Geometry/Geometry.h \
    $$PWD/FaceAlignment/FaceAlignment.h \
    $$PWD/FaceDetect/FaceDetect.h \
    $$PWD/FaceRecognition/FaceRecognition.h \
    $$PWD/FileOpt/FileOpt.h \
    $$PWD/StringOpt/StringOpt.h \
    $$PWD/SystemOpt/SystemOpt.h \
    $$PWD/IniWrapper/IniWrapper.h \
## ================================================================= ##
