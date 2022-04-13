#include "appscheduler.h"
#include "basemodel.h"

BaseModel::BaseModel() :
    m_pScheduler(nullptr)
{

}

BaseModel::~BaseModel()
{

}

int BaseModel::SetScheduler(AppScheduler *pScheduler)
{
    m_pScheduler = pScheduler;
    return 0;
}

int BaseModel::SendDataToDataAnnouncement(int cmdType, int dataLen, void *data)
{
    Announcement_t announcement;
    announcement.cmdType = cmdType;
    announcement.dataLen = dataLen;
    announcement.data = data;

    return SendDataTo(ANNOUNCEMENT, &announcement);
}

int BaseModel::SendDataToDataBase(int cmdType, int dataLen, void *data)
{
    DataBase_t dataBase;
    dataBase.cmdType = cmdType;
    dataBase.dataLen = dataLen;
    dataBase.data = data;

    return SendDataTo(DATABASE_MANAGER, &dataBase);
}

int BaseModel::SendDataToMainThread(int cmdType, int dataLen, void *data)
{
    MainThread_t mainThread;
    mainThread.cmdType = cmdType;
    mainThread.dataLen = dataLen;
    mainThread.data = data;

    return SendDataTo(MAIN_THREAD, &mainThread);
}

int BaseModel::SendDataToMsgAdapter(int cmdType, int dataLen, void *data)
{
    MsgAdapter_t msgAdapter;
    memset(&msgAdapter, 0, sizeof(MsgAdapter_t));
    msgAdapter.cmdType = cmdType;
    msgAdapter.dataLen = dataLen;
    msgAdapter.data    = data;

    return SendDataTo(MSG_ADAPTER, &msgAdapter);
}

int BaseModel::SendDataToUI(int tagPage, int cmdType, int dataLen, void *data)
{
    UIManager_t uiMgr;
    memset(&uiMgr, 0, sizeof(UIManager_t));
    uiMgr.tagPage = tagPage;
    uiMgr.cmdType = cmdType;
    uiMgr.dataLen = dataLen;
    uiMgr.data    = data;

    return SendDataTo(UI_MANAGER, &uiMgr);
}

int BaseModel::SendDataTo(Modeler mod, void *pData)
{
    if(m_pScheduler)
        return m_pScheduler->PosDataTo(mod, pData);
    else
        return -1;
}
