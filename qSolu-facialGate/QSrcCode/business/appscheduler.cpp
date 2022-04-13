#include "appscheduler.h"

#include "dataBase/dataBaseMgr.h"
#include "mainThread/mainThread.h"
#include "uiManager/uimanager.h"
#include "Announcement/announcement.h"
#include "msgAdapter/msgAdapter.h"

//#include "algorithmCtrl/algorithmCtrl.h"
//#include "doorContrl/doorcontrol.h"

extern int DataBaseMgrCallback(void *data);
extern int MainThreadCallback(void *data);
extern int UIManagerCallback(void *data);
extern int AnnouncementCallback(void *data);
extern int MsgAdapterCallback(void *data);

//extern int ALGOControlorCallback(void *data);
//extern int DoorCtrlCallback(void *data);

AppScheduler::AppScheduler()
{
    printf("----------------------start app-----------------------\n");
    InitBusinessModel();
    /// 1 - 创建数据库
    DataBaseMgr::createDataBaseMgr();
    //   1.1 - 注册数据库
    if(DataBaseMgr::instance()){
        DataBaseMgr::instance()->SetScheduler(this);
        Register(DATABASE_MANAGER, DataBaseMgrCallback);
    }

    /// 2 - 创建主逻辑业务块  - 用于执行周期性逻辑代码
    MainThread::createMainThread();
    //   2.1 - 注册主逻辑业务块
    if(MainThread::instance()){
        MainThread::instance()->SetScheduler(this);
        Register(MAIN_THREAD, MainThreadCallback);
    }

    /// 3 - 创建UI
    UIManager::createUIManager();
    //   3.1 - 注册UI
    if(UIManager::instance()){
        UIManager::instance()->SetScheduler(this);
        Register(UI_MANAGER, UIManagerCallback);
    }

    /// 4 - 创建语音播报器
    Announcement::createAnnouncement();
    //   4.1 - 注册语音播报器
    if(Announcement::instance()){
        Announcement::instance()->SetScheduler(this);
        Register(ANNOUNCEMENT, AnnouncementCallback);
    }

    /// 5 - 创建消息适配器  - 用于分发外部设备的协议处理
    MsgAdapter::createMsgAdapter();
    //   5.1 - 注册消息适配器
    if(MsgAdapter::instance()){
        MsgAdapter::instance()->init();
        MsgAdapter::instance()->SetScheduler(this);
        Register(MSG_ADAPTER, MsgAdapterCallback);
    }

#if 0
    /// 6 - 创建人脸识别管理模块 - 用于模块化识别流程的接口（暂未完善）
    ALGOControlor::createALGOControlor();
    //   6.1 - 注册RK人脸识别管理模块
    if(ALGOControlor::instance()){
        ALGOControlor::instance()->SetScheduler(this);
        Register(ALGO_CONTROLOR, ALGOControlorCallback);
        ALGOControlor::instance()->Init(UIManager::instance()->UIViewRect());
    }

    /// 7 - 创建DoorControl -- 用于喂狗
    DoorControl::createDoorControl();
    //   7.1 - 注册DoorContrl
    if(DoorControl::instance()){
        DoorControl::instance()->SetScheduler(this);
        Register(DOOR_CONTROL, DoorCtrlCallback);
    }
#endif
}

AppScheduler::~AppScheduler()
{

}

int AppScheduler::PosDataTo(Modeler mod, void *pData)
{
    BusinessCB pCBFunc = nullptr;
    map<int, BusinessCB>::iterator iter;

    iter = m_BusinessModel.find(mod);
    if(iter != m_BusinessModel.end()) {
        pCBFunc = iter->second;
    } else {
        qDebug("PosDataTo Model faild, Because Modeler %d isn't Register", mod);
        return -1;
    }

    if(pCBFunc)
        return pCBFunc(pData);
    else
        return -1;
}

int AppScheduler::InitBusinessModel()
{
    //初始化m_BusinessModel
    m_BusinessModel.clear();
    return 0;
}

int AppScheduler::Register(Modeler mod, BusinessCB pCBFunc)
{
    if((MODEL_HEAD < mod)&&(mod < MODEL_TAIL)){
        m_BusinessModel.insert(pair<int, BusinessCB>(mod, pCBFunc));
        qDebug("Register Model %d Succ ...", mod);
    }else{
        qDebug("Has Registered Model Faild, ModelIndex = %d", mod);
        return -1;
    }
    return 0;
}

int AppScheduler::UnRegister(Modeler mod)
{
    map<int, BusinessCB>::iterator iter;
    iter = m_BusinessModel.find(mod);

    if(iter != m_BusinessModel.end()){
        m_BusinessModel.erase(iter);
        qDebug("UnRegister Model %d Succ ...", mod);
        return 0;
    }else{
        qDebug("UnRegister Model faild, Because Modeler %d isn't Register", mod);
        return -1;
    }
}
