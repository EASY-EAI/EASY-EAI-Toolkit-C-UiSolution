#include "common/system.h"
#include "doorcontrol.h"

DoorControl *DoorControl::m_pSelf = NULL;

int DoorCtrlCallback(void *data)
{
    /*此回调处理其他模块送入的消息*/
    printf("dddddddddddddddddddddddddddd DoorCtrlCallback ...\n");
    DoorControl::instance()->doSomthing();
    return 0;
}

DoorControl::DoorControl()
{
    /*特别注意：不建议在构造函数内部使用跨模块调度(m_pScheduler->PosDataTo(mod,data)),目标业务模块有可能未被注册*/
}

DoorControl::~DoorControl()
{
    delete m_pSelf;
    m_pSelf = NULL;
}

void DoorControl::createDoorControl()
{
    if(m_pSelf == NULL)
        m_pSelf = new DoorControl;
}

int DoorControl::doSomthing()
{
    return 0;
}


