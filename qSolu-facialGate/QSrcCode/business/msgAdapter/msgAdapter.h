#ifndef __MSGADAPTER_H__
#define __MSGADAPTER_H__

#include <string>
#include <queue>

#include "business/basemodel.h"

class MsgAdapter;
//class UartControler;

typedef struct{
    MsgAdapter *pSelf;
}MsgAdapterPara_t;

class MsgAdapter : public BaseModel
{
public:
    explicit MsgAdapter();
    ~MsgAdapter();

    static MsgAdapter *instance() { return m_pSelf; }
    static void createMsgAdapter();
    void init();

    int sendDataToUart(void *data, int dataLen);
    int sendDataToHttp(void *data);

    /// handle >>>>: 处理其他模块过来输入的数据
    // exp: int func1(void);
    // exp: int func2(void);
private:

    static MsgAdapter *m_pSelf;
#if 0
    UartControler *m_pUartCtrl;
    int m_CardChn;
#endif

    MsgAdapterPara_t *mpMsgAdapterPara;
};


#endif // __MSGADAPTER_H__
