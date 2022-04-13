// ============================ Linux C ============================
#include "system.h"
// ========================== easyeai_api ==========================
#include "StringOpt/StringOpt.h"
// ============================ Project ============================
#include <QDebug>
#include "msgAdapter.h"


MsgAdapter *MsgAdapter::m_pSelf = nullptr;
int MsgAdapterCallback(void *data)
{
    /*此回调处理其他模块送入的消息*/
    int ret = -1;
    MsgAdapter_t msgAdapter;
    memcpy(&msgAdapter, data, sizeof(MsgAdapter_t));

    // handle
    if(MSG_ADAPTERCMD_NOQUERYDATA == msgAdapter.cmdType){
        ret = 0;
    }else if(MSG_ADAPTERCMD_PASSWORLD == msgAdapter.cmdType){
        ret = 0;
    }else if(MSG_ADAPTERCMD_QRCODE == msgAdapter.cmdType){
        ret = 0;
    }else if(MSG_ADAPTERCMD_FACEID == msgAdapter.cmdType){
        ret = 0;
    }

    return ret;
}

#if 0
/*串口模块上报上来的处理*/
#define MASTER_DEV          0x0001
#define SLAVE_DEV           0x0100
#define CMD_QUERY_ALL       0x10   //查询全部事件
#define CMD_NOTIFY          0x20   //弹框提醒
#define CMD_SETWORKMODE     0x30   //切换工作模式
#define CMD_STANDBY         0x31   //回到待机
#define CMD_REBOOT          0x32   //重启设备
#define CMD_DL_QRCODE       0x33   //下载二维码
#define CMD_DL_FACE_IMG     0x34   //下载证件图片
#define CMD_DEL_QUERY_FACE  0x35   //通过卡号 删除/查询 人脸
#define CMD_UPDATE_PUB      0x36   //更新公告栏
#define CMD_UPDATE_TIME     0x40   //更新时间
void UartRecvHandleCB(char *arg)
{
    //收
    std::string strCmd = arg;
    int i32SrcDev = 0;
    int i32CmdType = 0;
    int i32DataLen = 0;
    //发
    std::string strRetMsg;
    char cSendDataDev[5] = {'\0'};  //多给一个字节，防止std::string 的 append找不到'\0'
    char cSendCmdType[3] = {'\0'};  //多给一个字节，防止std::string 的 append找不到'\0'
    char cSendDataLen[5] = {'\0'};  //多给一个字节，防止std::string 的 append找不到'\0'
    int iSendDataLen = 0;

//    ascii2HexVal(&i32SrcDev,  (unsigned char *)strCmd.substr(1,4).c_str(), strCmd.substr(1,4).size());
//    ascii2HexVal(&i32CmdType, (unsigned char *)strCmd.substr(5,2).c_str(), strCmd.substr(5,2).size());
//    ascii2HexVal(&i32DataLen, (unsigned char *)strCmd.substr(7,4).c_str(), strCmd.substr(7,4).size());

    i32SrcDev =  hexToi((const char *)strCmd.substr(1,4).c_str());
    i32CmdType = hexToi((const char *)strCmd.substr(5,2).c_str());
    i32DataLen = hexToi((const char *)strCmd.substr(7,4).c_str());


    //收
    std::string strCmdContent;
    strCmdContent.clear();
    if(i32DataLen > 0)
        strCmdContent = strCmd.substr(11,i32DataLen);
    //发
    std::string strRetMsgContent;
    strRetMsgContent.clear();

    printf("i32SrcDev = 0x%02x\n",i32CmdType);
    if(MASTER_DEV == i32SrcDev){
        switch (i32CmdType) {
        case CMD_NOTIFY:
            printf("notify Msg\n");
            MsgAdapter::instance()->SendDataToUI(MSG_TAGPAGE_NOTIFY, MSG_UICMD_SHOWTEXT, i32DataLen, (char *)strCmdContent.c_str());
            break;
        case CMD_STANDBY:
            printf("switch to standby\n");
            MsgAdapter::instance()->SendDataToUI(MSG_TAGPAGE_STANDBY, MSG_UICMD_SHOW, 0, NULL);
            break;
        case CMD_UPDATE_PUB:
            printf("update publicy\n");
            MsgAdapter::instance()->SendDataToUI(MSG_TAGPAGE_STANDBY, MSG_UICMD_UPDATEPUBLICY, i32DataLen, (char *)strCmdContent.c_str());
            break;
        case CMD_UPDATE_TIME:
            printf("update date & time\n");
            MsgAdapter::instance()->SendDataToUI(MSG_TAGPAGE_STANDBY, MSG_UICMD_UPDATETIME, i32DataLen, (char *)strCmdContent.c_str());
            break;
        default:
            break;
        }

        //回发串口消息
        strRetMsg.append("F");
        val_to_hexStr(cSendDataDev, SLAVE_DEV, sizeof(cSendDataDev)-1);  //末尾'\0'不进行内存替换
        strRetMsg.append(cSendDataDev);
        val_to_hexStr(cSendCmdType, i32CmdType, sizeof(cSendCmdType)-1);  //末尾'\0'不进行内存替换
        strRetMsg.append(cSendCmdType);
        val_to_hexStr(cSendDataLen, iSendDataLen, sizeof(cSendDataLen)-1);  //末尾'\0'不进行内存替换
        strRetMsg.append(cSendDataLen);
        if(iSendDataLen > 0){
            strRetMsg.append(strRetMsgContent);
        }
        if(CMD_QUERY_ALL != i32CmdType){
            MsgAdapter::instance()->sendDataToUart((char *)strRetMsg.c_str(), strRetMsg.length());
        }
    }
}
#endif

#if 0
MsgAdapter::MsgAdapter() :
    m_pUartCtrl(nullptr),
    m_CardChn(-1)
#else
MsgAdapter::MsgAdapter()
#endif
{
    /*特别注意：不建议在构造函数内部使用跨模块调度(m_pScheduler->PosDataTo(mod,data)),目标业务模块有可能未被注册*/


#if 0
    //1.初始化http协议适配器

    //2.初始化串口管理器
    m_pUartCtrl = new UartControler();
    m_CardChn = m_pUartCtrl->getUartChannel("/dev/ttyS3", 57600);
    if(m_CardChn >= 0){
        m_pUartCtrl->setProtocolType(m_CardChn, "head_n_tail");
        m_pUartCtrl->setProtocolHead(m_CardChn, "gg");
        m_pUartCtrl->setProtocolTail(m_CardChn, "n");
        m_pUartCtrl->setCheckWay(m_CardChn, "BCC");
        m_pUartCtrl->setUartChnCB(m_CardChn, UartRecvHandleCB);
    }
#endif

    //3.资源初始化
    init();
}

MsgAdapter::~MsgAdapter()
{

}

void MsgAdapter::createMsgAdapter()
{
    if(m_pSelf == nullptr)
        m_pSelf = new MsgAdapter;
}

void MsgAdapter::init()
{

}

int MsgAdapter::sendDataToUart(void *data, int dataLen)
{
    Q_UNUSED(data);
    Q_UNUSED(dataLen);

#if 0
    if(m_CardChn >= 0)
        m_pUartCtrl->sentDataToUart(m_CardChn, data, dataLen);
#endif
    return 0;
}

int MsgAdapter::sendDataToHttp(void *data)
{
    Q_UNUSED(data);

    return 0;
}
