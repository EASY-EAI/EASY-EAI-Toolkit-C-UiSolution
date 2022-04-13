#ifndef __ALGOCONTROLOR_H__
#define __ALGOCONTROLOR_H__

#include "rkfacial/rkfacial.h"
#include "business/basemodel.h"

#include <rga/rga.h>
#include <rga/RgaApi.h>

#include <string>
#include <queue>

#include <QRect>

#define TWO_PLANE

typedef enum {
    ISP,
    CIF
} CAMERA_TYPE;

/******************************************************************************************************************
使用说明:
用例:
step1:
step2:
step3:
step4:
step5:
step6:
step7:
*******************************************************************************************************************/
class ALGOControlor : public BaseModel
{
public:
    explicit ALGOControlor(int refresh = 0);
    ~ALGOControlor();

    static ALGOControlor *instance() { return m_pSelf; }
    static void createALGOControlor();

    int Init(QRect screenRect, int faceCnt = 3000);
    bool isInited(){ return m_bIsInited; }

    QRect ScreenRect(){return m_ScreenRect;}

    int cameraSwitch();
    int registerSlots();
    int deleteSlots();
    void saveSlots();

    /// handle >>>>: 处理其他模块过来输入的数据
    void switchWorkMode(void *cpWorkMode);
    void setVolume(void *cpAnnouncements);


private:
    void saveFile(uchar *buf, int len, uchar *flag);

    static void paintBox(int left, int top, int right, int bottom);
    static void paintInfo(struct user_info *info, bool real);
    static void paintFace(int face_id);

    static void configRegion(int x, int y, int w, int h);

    static void displayRgb(void *src_ptr, int src_fd, RgaSURF_FORMAT src_fmt, int src_w, int src_h, int rotation);
    static void displayIr(void *src_ptr, int src_fd, RgaSURF_FORMAT src_fmt, int src_w, int src_h, int rotation);


    bool m_bIsInited;
    QRect m_ScreenRect;
    CAMERA_TYPE cameraType;
    bool saving;
    int saveFrames;
    int refreshFrame;

    int m_workMode;

    static ALGOControlor *m_pSelf;
};


#endif // __ALGOCONTROLOR_H__
