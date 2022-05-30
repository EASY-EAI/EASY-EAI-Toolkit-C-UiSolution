#ifndef __DATABASEMGR_H__
#define __DATABASEMGR_H__
// ============================ Common =============================
#include "dataBase/dataBase.h"

#include "business/basemodel.h"

class DataBaseMgr : public BaseModel
{
public:
    explicit DataBaseMgr();
    ~DataBaseMgr();

    static DataBaseMgr *instance()
    {
        if(m_pSelf == nullptr){
            once_flag oc;
            call_once(oc, [&] {
                m_pSelf = new DataBaseMgr;
            });
        }
        return m_pSelf;
    }
    static void createDataBaseMgr();

    void init();
    bool isInited(){ return mbIsInited; }

    int personNum(){return mPersonNum;}
    float *feature(int index);
    char *id(int index);
    void getName(int index, char *nameStr, int nameLen);

    int addRecordByIndex(int index, char *pName, char *pFeature, int featureLen);
    int addRecordById(char *pId, char *pName, char *pFeature, int featureLen);

    int clearDateBase();

    /// handle >>>>: 处理其他模块过来输入的数据
private:
    bool mbIsInited;
    int mPersonNum;
    faceData_t *mpFaceData;

    static DataBaseMgr *m_pSelf;
};


#endif // __DATABASEMGR_H__
