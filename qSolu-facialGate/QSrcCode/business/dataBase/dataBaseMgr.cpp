// ============================ Linux C ============================
#include "system.h"
// ============================ Project ============================
#include "dataBaseMgr.h"

DataBaseMgr *DataBaseMgr::m_pSelf = nullptr;

int DataBaseMgrCallback(void *data)
{
    /*此回调处理其他模块送入的消息*/
    int ret = -1;

    char *pCmdContent = NULL;
    DataBase_t dataBaseMgr;
    memcpy(&dataBaseMgr, data, sizeof(DataBase_t));

    if(0 < dataBaseMgr.dataLen && NULL != dataBaseMgr.data){
        pCmdContent = (char *)malloc(dataBaseMgr.dataLen + 1);
        memset(pCmdContent, 0, dataBaseMgr.dataLen + 1);
        memcpy(pCmdContent, dataBaseMgr.data, dataBaseMgr.dataLen);
    }

    // handle
    if(MSG_DBCMD_GETPERSONNUM == dataBaseMgr.cmdType){
        int *personNum = (int *)dataBaseMgr.data;
        *personNum = DataBaseMgr::instance()->personNum();
        ret = 0;
    }else if(MSG_DBCMD_GETFEATURE == dataBaseMgr.cmdType){
        FeatureData_t *pFD = (FeatureData_t *)dataBaseMgr.data;
        pFD->pFeature = DataBaseMgr::instance()->feature(pFD->pos);
        pFD->featureLen = 512;
    }else if(MSG_DBCMD_GETID == dataBaseMgr.cmdType){
        IdData_t *pIdData = (IdData_t *)dataBaseMgr.data;
        pIdData->pId = DataBaseMgr::instance()->id(pIdData->pos);
        pIdData->idLen = strlen(pIdData->pId);
    }else if(MSG_DBCMD_GETNAME == dataBaseMgr.cmdType){
        NameData_t *pNameD = (NameData_t *)dataBaseMgr.data;
        DataBaseMgr::instance()->getName(pNameD->pos, pNameD->name, sizeof(pNameD->name));
    }else if(MSG_DBCMD_ADD_REC_BYINDEX == dataBaseMgr.cmdType){
        AddRecordData_t *pRecordD = (AddRecordData_t *)dataBaseMgr.data;
        ret = DataBaseMgr::instance()->addRecordByIndex(pRecordD->index, pRecordD->pName, pRecordD->pFeature, pRecordD->featureLen);
    }else if(MSG_DBCMD_ADD_REC_BYID == dataBaseMgr.cmdType){
        AddRecordData_t *pRecordD = (AddRecordData_t *)dataBaseMgr.data;
        ret = DataBaseMgr::instance()->addRecordById(pRecordD->pId, pRecordD->pName, pRecordD->pFeature, pRecordD->featureLen);
    }

    if(pCmdContent)
        free(pCmdContent);

    return ret;
}

DataBaseMgr::DataBaseMgr() :
    mbIsInited(false),
    mPersonNum(-1),
    mpFaceData(nullptr)
{
    /*特别注意：不建议在构造函数内部使用跨模块调度(m_pScheduler->PosDataTo(mod,data)),目标业务模块有可能未被注册*/

    mpFaceData = (faceData_t *)malloc(MAX_USER_NUM * sizeof(faceData_t));
    memset(mpFaceData, 0, MAX_USER_NUM * sizeof(faceData_t));

    init();
}

DataBaseMgr::~DataBaseMgr()
{
    /* 数据库内存数据释放 */
    if(mpFaceData){
        free(mpFaceData);
        mpFaceData = nullptr;
    }

    /* 数据库释放 */
    database_exit();
}

void DataBaseMgr::createDataBaseMgr()
{
    if(m_pSelf == nullptr){
        once_flag oc;
        call_once(oc, [&] {
            m_pSelf = new DataBaseMgr;
        });
    }
}

void DataBaseMgr::init()
{
    // 初始化数据库
    database_init();
    // 同步数据库所有数据到内存
    if(mpFaceData){
        mPersonNum = database_getData_to_memory(mpFaceData);
    }

    if(mPersonNum >= 0)
        mbIsInited = true;
}

float *DataBaseMgr::feature(int index)
{
    float *pFeature = NULL;
    // 从内存中提取特征值
    if((mpFaceData) && (index >= 0))
        pFeature = (float *)((mpFaceData + index)->feature);

    return pFeature;
}

char *DataBaseMgr::id(int index)
{
    char *strId = NULL;
    if((mpFaceData) && (index >= 0))
        strId = ((mpFaceData + index)->id);

    return strId;
}

void DataBaseMgr::getName(int index, char *nameStr, int nameLen)
{
    // 从内存中提取用户名
    if((mpFaceData) && (index >= 0)){
//        printf("id : %s\n", (mpFaceData + index)->id);
        database_id_is_exist((mpFaceData + index)->id, nameStr, nameLen);
    }
}

int DataBaseMgr::addRecordByIndex(int index, char *pName, char *pFeature, int featureLen)
{
    if(NULL == mpFaceData){
        SendDataToDataAnnouncement(MSG_ANMCMD_REGISTFAILD, 0 ,NULL);
        return -1;
    }

    database_add_record((mpFaceData + index)->id, pName, pFeature, featureLen);
    mPersonNum = database_getData_to_memory(mpFaceData);

    SendDataToDataAnnouncement(MSG_ANMCMD_REGISTSUCC, 0 ,NULL);
    return 0;
}

int DataBaseMgr::addRecordById(char *pId, char *pName, char *pFeature, int featureLen)
{
    if(NULL == mpFaceData){
        SendDataToDataAnnouncement(MSG_ANMCMD_REGISTFAILD, 0 ,NULL);
        return -1;
    }

    database_add_record(pId, pName, pFeature, featureLen);
    mPersonNum = database_getData_to_memory(mpFaceData);

    SendDataToDataAnnouncement(MSG_ANMCMD_REGISTSUCC, 0 ,NULL);
    return 0;
}

int DataBaseMgr::clearDateBase()
{
    // 删除库
    database_delete_all_record();
    // 重载数据库
    mPersonNum = database_getData_to_memory(mpFaceData);

    return 0;
}

