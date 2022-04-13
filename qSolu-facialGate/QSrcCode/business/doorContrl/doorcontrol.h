#ifndef DOORCONTROL_H
#define DOORCONTROL_H

#include "business/basemodel.h"

class DoorControl : public BaseModel
{
public:
    explicit DoorControl();
    ~DoorControl();

    static DoorControl *instance() { return m_pSelf; }
    static void createDoorControl();

    /// handle >>>>: 处理其他模块过来输入的数据
    int doSomthing();
private:

    static DoorControl *m_pSelf;
};

#endif // DOORCONTROL_H
