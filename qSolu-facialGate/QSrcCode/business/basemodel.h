#ifndef __BASEMODEL_H__
#define __BASEMODEL_H__

#include "bsprotocol.h"
class AppScheduler;

class BaseModel
{
public:
    explicit BaseModel();
    ~BaseModel();
    int SetScheduler(AppScheduler *pScheduler);

    /// 输出>>>>: 向其他模块输出数据
    virtual int SendDataToDataAnnouncement(int cmdType, int dataLen, void *data);
    virtual int SendDataToDataBase(int cmdType, int dataLen, void *data);
    virtual int SendDataToMainThread(int cmdType, int dataLen, void *data);
    virtual int SendDataToMsgAdapter(int cmdType, int dataLen, void *data);
    virtual int SendDataToUI(int tagPage, int cmdType, int dataLen, void *data);

private:
    int SendDataTo(Modeler mod, void *pData);

    AppScheduler *m_pScheduler;
};


#endif // __BASEMODEL_H__
