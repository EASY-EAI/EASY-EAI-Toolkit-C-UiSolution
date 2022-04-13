#ifndef __MAINTHREAD_H__
#define __MAINTHREAD_H__

#include "business/basemodel.h"
#include <opencv2/opencv.hpp>

using namespace cv;

class MainThread;
typedef struct{
    MainThread *pSelf;
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    uint8_t color[3];
}MainThreadPara_t;

class MainThread : public BaseModel
{
public:
    explicit MainThread();
    ~MainThread();

    static MainThread *instance() { return m_pSelf; }
    static void createMainThread();

    // AddRecord flag operation
    bool needAddRecord(){return mbNeedAddRec;}

    // Lock operation
    void initImgLock();
    void deInitImgLock();
    void imgLock();
    void imgUnLock();

    /// handle >>>>: 处理其他模块过来输入的数据
    int setNeedAddRec(bool bIsTrue);

    Mat mAlgoImage;
private:
    bool mbNeedAddRec;

    pthread_mutex_t mImgLock;

    static MainThread *m_pSelf;
    MainThreadPara_t *mpMainThreadPara;
};

#endif // __MAINTHREAD_H__
