#include "common/system.h"
#include "common/interface.h"
#include "algorithmCtrl.h"

#include <iostream>
#include <QtWidgets>
#include <QTouchEvent>
#include <QList>

#include <dbserver.h>
#include <rkfacial/rkfacial.h>
#ifdef TWO_PLANE
#include <rkfacial/display.h>
#include <rkfacial/draw_rect.h>
#endif

// prj

#define SAVE_FRAMES 30

ALGOControlor *ALGOControlor::m_pSelf = nullptr;

// 判断矩形是否合法
static bool coordIsVaild(int left, int top, int right, int bottom)
{
    if(left < 0 || top < 0 || right < 0 || bottom < 0) {
        qDebug("%s: invalid rect(%d, %d, %d, %d)", __func__, left, top, right, bottom);
        return false;
    }

    if(left > right || top > bottom) {
        qDebug("%s: invalid rect(%d, %d, %d, %d)", __func__, left, top, right, bottom);
        return false;
    }

    return true;
}

int ALGOControlorCallback(void *data)
{
    /*此回调处理其他模块送入的消息*/
    char *pCmdContent = NULL;
    RKFacial_t rkfMgr;
    memcpy(&rkfMgr, data, sizeof(RKFacial_t));

    if(0 < rkfMgr.dataLen && NULL != rkfMgr.data){
        pCmdContent = malloc(rkfMgr.dataLen + 1);
        memset(pCmdContent, '\0', rkfMgr.dataLen + 1);
        memcpy(pCmdContent, rkfMgr.data, rkfMgr.dataLen);
    }

    switch (rkfMgr.cmdType) {
    case MSG_RKFCMD_SETWORKMODE:
        ALGOControlor::instance()->switchWorkMode(pCmdContent);
        break;
    case MSG_RKFCMD_SETVOLUME:
        ALGOControlor::instance()->setVolume(pCmdContent);
        break;
    default:
        break;
    }

    if(pCmdContent)
        free(pCmdContent);

    return 0;
}

ALGOControlor::ALGOControlor(int refresh) :
    m_bIsInited(false),
    cameraType(ISP),
    saving(false),
    saveFrames(SAVE_FRAMES),
    refreshFrame(refresh)
{
    /*特别注意：不建议在构造函数内部使用跨模块调度(m_pScheduler->PosDataTo(mod,data)),目标业务模块有可能未被注册*/
}

ALGOControlor::~ALGOControlor()
{
    rkfacial_exit();
    display_exit();
}

void ALGOControlor::createALGOControlor()
{
    if(m_pSelf == nullptr)
        m_pSelf = new ALGOControlor(0);
}

void ALGOControlor::switchWorkMode(void *cpWorkMode)
{
    if(NULL == cpWorkMode)
        return ;

    int workMode = 0;
    std::string strWorkMode = (char *)cpWorkMode;
//    ascii2HexVal(&workMode,  (unsigned char *)strWorkMode.c_str(), strWorkMode.size());
    workMode =  hexToi((const char *)strWorkMode.c_str());

    switch (workMode) {
    case 0x01:  //普通模式：人脸靠近，并测温
        SendDataToUI(MSG_TAGPAGE_STANDBY, MSG_UICMD_SHOW, 0, NULL);
        break;
    case 0x02:  //键盘输入：弹出键盘输入页
        SendDataToUI(MSG_TAGPAGE_INPUT, MSG_UICMD_SHOWKEYBOARD, 0, NULL);
        break;
    case 0x03:  //人证比对：人脸靠近，并测温
        break;
    default:
        break;
    }
    m_workMode = workMode;

}

void ALGOControlor::setVolume(void *cpAnnouncements)
{
    RKF_Announcements_t Announcements;

    if(NULL == cpAnnouncements)
        return ;

    memcpy(&Announcements, cpAnnouncements, sizeof(RKF_Announcements_t));
}

int ALGOControlor::Init(QRect screenRect, int faceCnt)
{
//    pthread_t rPid, wPid;
    m_ScreenRect = screenRect;

#ifdef TWO_PLANE
    set_rgb_param(CAMERA_WIDTH, CAMERA_HEIGHT, NULL, true);
    set_ir_param(CAMERA_WIDTH, CAMERA_HEIGHT, NULL);
    set_rgb_rotation(90);

    display_switch(DISPLAY_VIDEO_RGB);

//    cout<<"m_ScreenRect.width():"<<m_ScreenRect.width()<<endl;
//    cout<<"m_ScreenRect.height():"<<m_ScreenRect.height()<<endl;


    if (display_init(m_ScreenRect.width(), m_ScreenRect.height())) {
        qDebug("%s: display_init failed", __func__);
        return -1;
    }

#else
    set_rgb_param(CAMERA_WIDTH, CAMERA_HEIGHT, displayRgb, true);
    set_ir_param(CAMERA_WIDTH, CAMERA_HEIGHT, displayIr);
#endif

    set_face_param(CAMERA_WIDTH, CAMERA_HEIGHT, faceCnt);

    register_rkfacial_paint_box(paintBox);
    register_rkfacial_paint_info(paintInfo);
    register_rkfacial_paint_face(paintFace);
    //register_get_face_config_region(configRegion);


    if(rkfacial_init() < 0) {
        qDebug("%s: rkfacial_init failed", __func__);
        return -1;
    }

    m_bIsInited = true;
    return 0;
}

int ALGOControlor::cameraSwitch()
{
    if(cameraType == ISP) {
        // 这里要想办法去耦
//        UIManager::instance()->VideoItem_instance()->setBoxRect(0, 0, -1, -1);
        cameraType = CIF;

    } else {
        cameraType = ISP;

    }
    return 0;
}

int ALGOControlor::registerSlots()
{
    rkfacial_register();
    return 0;
}

int ALGOControlor::deleteSlots()
{
    rkfacial_delete();
    // 这里要想办法去耦
//    UIManager::instance()->VideoItem_instance()->clear();
    return 0;
}

void ALGOControlor::saveSlots()
{
    saving = true;
}

void ALGOControlor::saveFile(uchar *buf, int len, uchar *flag)
{
    if(!saving)
        return;

    if(saveFrames) {
        SaveThread *thread = new SaveThread(buf, len, flag, saveFrames);
        thread->start();
        saveFrames--;
    } else {
        saveFrames = SAVE_FRAMES;
        saving = false;
    }
}

void ALGOControlor::paintBox(int left, int top, int right, int bottom)
{
    if(!coordIsVaild(left, top, right, bottom))
        return;

    display_paint_box(left, top, right, bottom);
    display_set_color(set_yuv_color(COLOR_G));

    // 这里要想办法去耦---封装一个统一的回调函数
    //UIManager::instance()->UpdateUI(0, 0, mpRKFCtrl->ScreenRect().width(), mpRKFCtrl->ScreenRect().height());
    return;

}

void ALGOControlor::paintInfo(user_info *info, bool real)
{
    // 这里要想办法去耦---封装一个统一的回调函数
    //UIManager::instance()->VideoItem_instance()->setUserInfo(info, real);
    //UIManager::instance()->UpdateUI(0, 0, mpRKFCtrl->ScreenRect().width(), mpRKFCtrl->ScreenRect().height());
}

void ALGOControlor::paintFace(int face_id)
{
    //Mat input_ptr = Mat(height, width, CV_8UC3, ptr);
//    desktopView->videoItem->paintFaceInfo(face_id);
    play_wav_signal(PLEASE_GO_THROUGH_WAV);
//    SendDataToUI();
}

void ALGOControlor::configRegion(int x, int y, int w, int h)
{
    // 这里要想办法去耦---封装一个统一的回调函数
    //UIManager::instance()->VideoItem_instance()->setRegion(x, y, w, h);
    //UIManager::instance()->UpdateUI(0, 0, mpRKFCtrl->ScreenRect().width(), mpRKFCtrl->ScreenRect().height());
}

void ALGOControlor::displayRgb(void *src_ptr, int src_fd, RgaSURF_FORMAT src_fmt, int src_w, int src_h, int rotation)
{
    // 这里要想办法去耦---封装一个统一的回调函数
    //UIManager::instance()->VideoItem_instance()->render((uchar *)src_ptr, src_fmt, rotation, src_w, src_h);
    //UIManager::instance()->UpdateUI(0, 0, mpRKFCtrl->ScreenRect().width(), mpRKFCtrl->ScreenRect().height());
}

void ALGOControlor::displayIr(void *src_ptr, int src_fd, RgaSURF_FORMAT src_fmt, int src_w, int src_h, int rotation)
{

//    saveFile((uchar *)src_ptr, src_w * src_h * 3 / 2, "ir");

    // 这里要想办法去耦---封装一个统一的回调函数
    //UIManager::instance()->VideoItem_instance()->render((uchar *)src_ptr, src_fmt, rotation,src_w, src_h);
    //UIManager::instance()->UpdateUI(0, 0, mpRKFCtrl->ScreenRect().width(), mpRKFCtrl->ScreenRect().height());
//    SendDataToUI();
}
