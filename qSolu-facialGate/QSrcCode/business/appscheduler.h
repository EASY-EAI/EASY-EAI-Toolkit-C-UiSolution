#ifndef APPSCHEDULER_H
#define APPSCHEDULER_H

#include "bsprotocol.h"
#include <QDebug>

class AppScheduler
{
public:
    AppScheduler();
    ~AppScheduler();

    int PosDataTo(Modeler mod, void *pData);

private:
    int InitBusinessModel();
    int Register(Modeler mod, BusinessCB pCBFunc);
    int UnRegister(Modeler mod);

    std::map<int, BusinessCB> m_BusinessModel;
};

#endif // APPSCHEDULER_H
