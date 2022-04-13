#ifndef __BSPROTOCOL_H__
#define __BSPROTOCOL_H__

#include "system.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

#define QRCODE_PATH            "/userdata/QFacePass1109_release/QRCode.png"
#define IDCARD_FACEIMG_PATH    "/userdata/QFacePass1109_release/idcard.bmp"

enum Modeler{
    MODEL_HEAD = 0,
    MAIN_THREAD,
    MSG_ADAPTER,
    UI_MANAGER,
    DATABASE_MANAGER,
    ANNOUNCEMENT,
    DOOR_CONTROL,
    ALGO_CONTROLOR,
    MODEL_TAIL
};

typedef	int (*BusinessCB)(void *);

//===================  MsgAdapter  ===========================
#define MSG_ADAPTERCMD_NOQUERYDATA    0x01
#define MSG_ADAPTERCMD_PASSWORLD      0x02
#define MSG_ADAPTERCMD_QRCODE         0x03
#define MSG_ADAPTERCMD_FACEID         0x04
typedef struct {
    int cmdType;    //做什么
    int dataLen;    //怎么做
    void *data;
}MsgAdapter_t;

//=======================  UI  ===============================
#define MSG_TAGPAGE_STANDBY    (0x01)
#define   MSG_UICMD_SHOW                0x01
#define   MSG_UICMD_UPDATEPUBLICY       0x02
#define   MSG_UICMD_UPDATETIME          0x03
#define MSG_TAGPAGE_NOTIFY     (0x02)
#define   MSG_UICMD_SHOWTEXT          0x01
#define MSG_TAGPAGE_INPUT      (0x03)
#define   MSG_UICMD_SHOWKEYBOARD      0x01
typedef struct {
    int left;
    int top;
    int right;
    int bottom;
}Rect_t;
typedef struct {
    char textColor[32];
    char imagePath[128];
    char tips[128];
    char name[64];
    char id[64];
}NotifyData_t;
typedef struct {
    int tagPage;    //目的地
    int cmdType;    //做什么
    int dataLen;    //怎么做
    void *data;
}UIManager_t;

//====================  mainThread  ============================
#define MSG_MTCMD_ADDRECORD          0x01
#define MSG_RKFCMD_SETVOLUME         0x02
typedef struct {
    int cmdType;    //做什么
    int dataLen;    //怎么做
    void *data;
}MainThread_t;

//=====================  DataBase  =============================
#define MSG_DBCMD_GETPERSONNUM       0x01
#define MSG_DBCMD_GETFEATURE         0x02
#define MSG_DBCMD_GETID              0x03
#define MSG_DBCMD_GETNAME            0x04
#define MSG_DBCMD_ADD_REC_BYINDEX    0x05
#define MSG_DBCMD_ADD_REC_BYID       0x06
#define MSG_DBCMD_SENDPASSWORD       0x07
typedef struct {
    int pos;
    float *pFeature;
    int featureLen;
}FeatureData_t;
typedef struct {
    int pos;
    int idLen;
    char *pId;
}IdData_t;
typedef struct {
    int pos;
    char name[64];
}NameData_t;
typedef struct {
    int index;
    char *pId;
    char *pName;
    char *pFeature;
    int featureLen;
}AddRecordData_t;
typedef struct {
    int cmdType;    //做什么
    int dataLen;    //怎么做
    void *data;
}DataBase_t;

//=====================  Announcement  =============================
#define MSG_ANMCMD_SETVOLUME         0x01
#define MSG_ANMCMD_WELCOME           0x02
#define MSG_ANMCMD_REGISTSUCC        0x03
#define MSG_ANMCMD_REGISTFAILD       0x04
#define MSG_ANMCMD_PASS              0x05
typedef struct {
    bool bIsNeedtoPlay;
    int volume;
}VolumeData_t;
typedef struct {
    int cmdType;    //做什么
    int dataLen;    //怎么做
    void *data;
}Announcement_t;


#endif // __BSPROTOCOL_H__
