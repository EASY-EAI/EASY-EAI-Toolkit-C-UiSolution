// ============================ Linux C ============================
#include "system.h"
// ========================== easyeai_api ==========================
#include "Geometry/Geometry.h"
#include "FaceAlignment/FaceAlignment.h"
#include "FaceDetect/FaceDetect.h"
#include "FaceRecognition/FaceRecognition.h"
#include "FileOpt/FileOpt.h"
#include "SystemOpt/SystemOpt.h"
#include "Network/Network.h"
#include "Camera/Camera.h"
#include "Display/Display.h"
// ============================ Project ============================
#include "mainThread.h"

#define IMAGE_PATH "./images/"

MainThread *MainThread::m_pSelf = NULL;

void *AnalysisThreadBody(void *arg)
{
    MainThreadPara_t *pPara = (MainThreadPara_t *)arg;
    MainThread *pSelf = pPara->pSelf;

    if(nullptr == pSelf){
        printf("error : pSelf is null ...\n");
        pthread_exit(nullptr);
    }

    int ret = -1;

    // 初始化人脸检测
    rknn_context detect_ctx;
    std::vector<det> detect_result;
    Point2f points[5];
    s32Rect_t irRect, rgbRect;
    printf("face detect init!\n");
    ret = face_detect_init(&detect_ctx, "./face_detect.model");
    if( ret < 0){
        printf("face_detect fail! ret=%d\n", ret);
        return NULL;
    }
    // 初始化人脸识别
    rknn_context recognition_ctx;
    float face_feature[512];
    printf("face recognition init!\n");
    ret =  face_recognition_init(&recognition_ctx, "./face_recognition.model");
    if( ret < 0){
        printf("face_recognition fail! ret=%d\n", ret);
        return NULL;
    }

    // 获取数据库已注册人数
    int personNum = -1;
    MainThread::instance()->SendDataToDataBase(MSG_DBCMD_GETPERSONNUM, sizeof(personNum), &personNum);

    uint64_t start_time, end_time;
    Mat image,irImage;
    Mat face_align;
    float similarity; //特征值相似度比对
    int face_index = 0;
    while(1)
    {
        if(pSelf->mAlgoImage.empty() || pSelf->mAlgoIRImage.empty()) {
            usleep(5);
            continue;
        }
        pSelf->imgLock();
        irImage = pSelf->mAlgoIRImage.clone();
        image = pSelf->mAlgoImage.clone();
        pSelf->imgUnLock();

        // 活体检测，计算出人脸位置
        ret = face_detect_run(detect_ctx, irImage, detect_result);
        if(ret <= 0){   //非活体
            // 识别结果数据，复位
            pPara->x = 0; pPara->y = 0; pPara->w = 0; pPara->h = 0;
            pSelf->setNeedAddRec(false);
            usleep(1000);
            continue;
        }
        irRect.left   = (uint32_t)(detect_result[0].box.x);
        irRect.top    = (uint32_t)(detect_result[0].box.y);
        irRect.right  = (uint32_t)(detect_result[0].box.x + detect_result[0].box.width);
        irRect.bottom = (uint32_t)(detect_result[0].box.y + detect_result[0].box.height);

        // 人脸检测，计算出人脸位置
        ret = face_detect_run(detect_ctx, image, detect_result);
        if(ret <= 0){
            // 识别结果数据，复位
            pPara->x = 0; pPara->y = 0; pPara->w = 0; pPara->h = 0;
            pSelf->setNeedAddRec(false);
            usleep(1000);
            continue;
        }
        pPara->x = (uint32_t)(detect_result[0].box.x);
        pPara->y = (uint32_t)(detect_result[0].box.y);
        pPara->w = (uint32_t)(detect_result[0].box.width);
        pPara->h = (uint32_t)(detect_result[0].box.height);
        rgbRect.left   = pPara->x;
        rgbRect.top    = pPara->y;
        rgbRect.right  = pPara->x + pPara->w;
        rgbRect.bottom = pPara->y + pPara->h;
#if 0
        // 计算ir人脸与rgb人脸框重合度(最高为1.0)
        printf("ir[(%d, %d)--(%d, %d)] rgb[(%d, %d)--(%d, %d)],  IoU is %lf\n", irRect.left, irRect.top, irRect.right, irRect.bottom
                                                                              , rgbRect.left, rgbRect.top, rgbRect.right, rgbRect.bottom
                                                                              , calc_intersect_of_union(irRect, rgbRect));
#endif
        if(calc_intersect_of_union(irRect, rgbRect) <= 0.5){
            // 识别结果数据，复位
            pPara->x = 0; pPara->y = 0; pPara->w = 0; pPara->h = 0;
            pSelf->setNeedAddRec(false);
            usleep(1000);
            continue;
        }

        for (int i = 0; i < (int)detect_result[0].landmarks.size(); ++i) {
            points[i].x = (int)detect_result[0].landmarks[i].x;
            points[i].y = (int)detect_result[0].landmarks[i].y;
        }
        // 人脸校正(从图像中裁出人脸)
        face_align = face_alignment(image, points);
        // 人脸识别，计算特征值
        start_time = get_timeval_ms();
        face_recognition_run(recognition_ctx, &face_align, &face_feature);
        end_time = get_timeval_ms();
        printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        printf("face_recognition_run use time: %llu\n", end_time - start_time);
        // 特征值比对，得出id
        similarity = -0.5;
        if(personNum > 0){
            FeatureData_t featureData;
            memset(&featureData, 0, sizeof(featureData));
            for(face_index = 0; face_index < personNum; ++face_index){
                // 从数据库取出特征值
                featureData.pos = face_index;
                MainThread::instance()->SendDataToDataBase(MSG_DBCMD_GETFEATURE, sizeof(featureData), &featureData);
                // 比对
                similarity = face_recognition_comparison(face_feature, featureData.pFeature, featureData.featureLen);
                if(similarity > 0.5) {break;}
            }
        }
        printf("similarity:%f\n", similarity);
        if((face_index < personNum)&&(similarity > 0.5)){
            // 显示绿框
            pPara->color[0] = 0;
            pPara->color[1] = 255;
            pPara->color[2] = 0;
            // 从数据库获取名字
            NameData_t nameData;
            memset(&nameData, 0, sizeof(nameData));
            nameData.pos = face_index;
            MainThread::instance()->SendDataToDataBase(MSG_DBCMD_GETNAME, sizeof(nameData), &nameData);

            IdData_t idData;
            idData.pos = face_index;
            MainThread::instance()->SendDataToDataBase(MSG_DBCMD_GETID, sizeof(idData), &idData);

            char face_path[128] = {0};
            sprintf(face_path, "%s%s.jpeg", IMAGE_PATH, nameData.name);

            NotifyData_t notify;
            memset(&notify, 0, sizeof(NotifyData_t));
            sprintf(notify.textColor, "FFFFFF");
            sprintf(notify.tips, "通知: 识别成功");
            sprintf(notify.id, "ID: %s", idData.pId);
            memcpy(notify.imagePath, face_path, strlen(face_path));
            memcpy(notify.name, nameData.name, strlen(nameData.name));
            MainThread::instance()->SendDataToUI(MSG_TAGPAGE_NOTIFY, MSG_UICMD_SHOWTEXT, sizeof(notify), &notify);

            // 按键被按下，更新特征值
            if(pSelf->needAddRecord()){
                pSelf->setNeedAddRec(false);
                // 特征值入库
                AddRecordData_t recData;
                memset(&recData, 0, sizeof(recData));
                recData.index = face_index;
                recData.pName = nameData.name;
                recData.pFeature = (char *)face_feature;
                recData.featureLen = sizeof(face_feature);
                ret = MainThread::instance()->SendDataToDataBase(MSG_DBCMD_ADD_REC_BYINDEX, sizeof(recData), &recData);
                if(0 == ret){
                    imwrite(face_path, face_align);
                }
                // 更新数据库人数
                MainThread::instance()->SendDataToDataBase(MSG_DBCMD_GETPERSONNUM, sizeof(personNum), &personNum);
            }
        }else{
            // 显示红框
            pPara->color[0] = 0;
            pPara->color[1] = 0;
            pPara->color[2] = 255;
            // 按键被按下，录入特征值
            if(pSelf->needAddRecord()){
                pSelf->setNeedAddRec(false);
                // 特征值入库
                char idStr[32]={0};
                char nameStr[32]={0};
                sprintf(idStr, "%05d", face_index+1);
                sprintf(nameStr, "people_%d", face_index+1);
                AddRecordData_t recData;
                memset(&recData, 0, sizeof(recData));
                recData.pId = idStr;
                recData.pName = nameStr;
                recData.pFeature = (char *)face_feature;
                recData.featureLen = sizeof(face_feature);
                ret = MainThread::instance()->SendDataToDataBase(MSG_DBCMD_ADD_REC_BYID, sizeof(recData), &recData);
                if(0 == ret){
                    char face_path[128] = {0};
                    sprintf(face_path, "%s%s.jpeg", IMAGE_PATH, nameStr);
                    imwrite(face_path, face_align);
                }
                // 更新数据库人数
                MainThread::instance()->SendDataToDataBase(MSG_DBCMD_GETPERSONNUM, sizeof(personNum), &personNum);
            }
        }
        usleep(16*1000);
    }
    /* 人脸识别释放 */
    face_recognition_release(recognition_ctx);
    /* 人脸检测释放 */
    face_detect_release(detect_ctx);
    return NULL;
}

void *CapAndDisThreadBody(void *arg)
{
    MainThreadPara_t *pPara = (MainThreadPara_t *)arg;
    MainThread *pSelf = pPara->pSelf;

    if(nullptr == pSelf){
        printf("error : pSelf is null ...\n");
        pthread_exit(nullptr);
    }

    int skip = 0;
    int rgbRet,irRet;
    int ret = 0;
    char *pRGBbuf = NULL;
    char *pIRbuf = NULL;

    Mat image;

    // 1.打开摄像头
#define CAMERA_WIDTH	720
#define CAMERA_HEIGHT	1280
#define	IMGRATIO		3
#define	IMAGE_SIZE		(CAMERA_WIDTH*CAMERA_HEIGHT*IMGRATIO)
    ret = rgbcamera_init(CAMERA_WIDTH, CAMERA_HEIGHT, 270);
    if (ret) {
        printf("error: %s, %d\n", __func__, __LINE__);
        goto exit4;
    }
    ret = ircamera_init(CAMERA_WIDTH, CAMERA_HEIGHT, 270);
    if (ret) {
        printf("error: %s, %d\n", __func__, __LINE__);
        goto exit3;
    }

    pRGBbuf = (char *)malloc(IMAGE_SIZE);
    if (!pRGBbuf) {
        printf("error: %s, %d\n", __func__, __LINE__);
        ret = -1;
        goto exit3;
    }
    pIRbuf = (char *)malloc(IMAGE_SIZE);
    if (!pIRbuf) {
        printf("error: %s, %d\n", __func__, __LINE__);
        ret = -1;
        goto exit2;
    }

    // 跳过前10帧
    skip = 10;
    while(skip--) {
        ret = rgbcamera_getframe(pRGBbuf);
        if (ret) {
            printf("error: %s, %d\n", __func__, __LINE__);
            goto exit1;
        }
        ret = rgbcamera_getframe(pIRbuf);
        if (ret) {
            printf("error: %s, %d\n", __func__, __LINE__);
            goto exit1;
        }
    }

    // 2.显示初始化
#define SCREEN_WIDTH	720
#define SCREEN_HEIGHT	1280
    disp_preset_uiLayer(SYS_TRUE);
    ret = disp_init(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (ret) {
        printf("error: %s, %d\n", __func__, __LINE__);
        goto exit1;
    }
    // 3.(取流 + 显示)循环
    while(1){
        // 3.1、取流
        pSelf->imgLock();
        rgbRet = rgbcamera_getframe(pRGBbuf);
        irRet  = ircamera_getframe(pIRbuf);
        if ((0 != rgbRet) || (0 != irRet)) {
            printf("error: %s, %d\n", __func__, __LINE__);
            pSelf->imgUnLock();
            continue;
        }
        pSelf->mAlgoIRImage = Mat(CAMERA_HEIGHT, CAMERA_WIDTH, CV_8UC3, pIRbuf);
        pSelf->mAlgoImage = Mat(CAMERA_HEIGHT, CAMERA_WIDTH, CV_8UC3, pRGBbuf);
        image = pSelf->mAlgoImage.clone();
//        image = pSelf->mAlgoIRImage.clone();
        pSelf->imgUnLock();

        // 3.2、显示
        // 画框
        rectangle(image, Rect(pPara->x, pPara->y,pPara-> w, pPara->h), Scalar(pPara->color[0], pPara->color[1], pPara->color[2]), 3);
        disp_commit(image.data, IMAGE_SIZE);

        usleep(20*1000);
    }

exit1:
    free(pIRbuf);
    pIRbuf = NULL;
exit2:
    free(pRGBbuf);
    pRGBbuf = NULL;
    ircamera_exit();
exit3:
    rgbcamera_exit();
exit4:
    return NULL;
}

int MainThreadCallback(void *data)
{
    /*此回调处理其他模块送入的消息*/
    int ret = -1;

    MainThread_t mainThread;
    memcpy(&mainThread, data, sizeof(DataBase_t));

    // handle
    if(MSG_MTCMD_ADDRECORD == mainThread.cmdType){
        if(!(0 < mainThread.dataLen && NULL != mainThread.data)){ return ret; }

        int *pData = (int *)mainThread.data;
        printf("set Need Add Rec %d\n", *pData);
        ret = MainThread::instance()->setNeedAddRec((bool)*pData);
    }

    return ret;
}

MainThread::MainThread() :
    mbNeedAddRec(false),
    mpMainThreadPara(nullptr)
{
    /*特别注意：不建议在构造函数内部使用跨模块调度(m_pScheduler->PosDataTo(mod,data)),目标业务模块有可能未被注册*/
    pthread_t CnDPid;
    pthread_t AnalysisPid;
    if (!mpMainThreadPara) {
        mpMainThreadPara = (MainThreadPara_t *)malloc(sizeof(MainThreadPara_t));
    }
    if(mpMainThreadPara) {
        memset(mpMainThreadPara, 0, sizeof(MainThreadPara_t));
        mpMainThreadPara->pSelf = this;
    }

    // 创建用于存放用户头像的目录
    char mkPathCmd[128]={0};
    sprintf(mkPathCmd, "mkdir %s", IMAGE_PATH);
    exec_cmd_by_system(mkPathCmd);
    //make_directory(IMAGE_PATH);

    // 创建图像互斥锁
    initImgLock();

    // 创建视频采集，显示线程
    if(0 != CreateNormalThread(CapAndDisThreadBody, mpMainThreadPara, &CnDPid)){
        free(mpMainThreadPara);
        mpMainThreadPara = nullptr;
        return;
    }

    // 创建算法分析线程
    if(0 != CreateNormalThread(AnalysisThreadBody, mpMainThreadPara, &AnalysisPid)){
        free(mpMainThreadPara);
        mpMainThreadPara = nullptr;
        return;
    }
}

MainThread::~MainThread()
{
    deInitImgLock();

    if(mpMainThreadPara){
        free(mpMainThreadPara);
        mpMainThreadPara = nullptr;
    }

    delete m_pSelf;
    m_pSelf = NULL;
}

void MainThread::createMainThread()
{
    if(m_pSelf == nullptr){
        once_flag oc;
        call_once(oc, [&] {
            m_pSelf = new MainThread;
        });
    }
}

void MainThread::initImgLock()
{
    pthread_mutex_init(&mImgLock, NULL);
}

void MainThread::deInitImgLock()
{
    pthread_mutex_destroy(&mImgLock);
}

void MainThread::imgLock()
{
    pthread_mutex_lock(&mImgLock);
}

void MainThread::imgUnLock()
{
    pthread_mutex_unlock(&mImgLock);
}

int MainThread::setNeedAddRec(bool bIsTrue)
{
    mbNeedAddRec = bIsTrue;
    return 0;
}
