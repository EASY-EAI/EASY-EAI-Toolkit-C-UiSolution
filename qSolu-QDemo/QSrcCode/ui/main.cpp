#include <QApplication>
#include <QTextCodec>

#include "system.h"

#include "SystemOpt/SystemOpt.h"
#include "Camera/Camera.h"
#include "Display/Display.h"

#include "mainWidget.h"

#define CAMERA_WIDTH	720
#define CAMERA_HEIGHT	1280
#define	DISP_WIDTH		720
#define	DISP_HEIGHT		1280
#define	IMGRATIO		3
#define	IMAGE_SIZE		(CAMERA_WIDTH*CAMERA_HEIGHT*IMGRATIO)
void *displayCamera(void *para)
{
    char *pbuf = NULL;
    int ret = 0;
    ret = rgbcamera_init(CAMERA_WIDTH, CAMERA_HEIGHT, 90);
    if (ret) {
        printf("error func:%s, line:%d\n", __func__, __LINE__);
        goto exit3;
    }

    disp_preset_uiLayer(SYS_TRUE);
    ret = disp_init(DISP_WIDTH, DISP_HEIGHT); //RGB888 default
    if (ret) {
        printf("error func:%s, line:%d\n", __func__, __LINE__);
        goto exit2;
    }
    pbuf = (char *)malloc(IMAGE_SIZE);
    if (!pbuf) {
        printf("error: %s, %d\n", __func__, __LINE__);
        ret = -1;
        goto exit1;
    }

    while(1) {
        ret = rgbcamera_getframe(pbuf);
        if (!ret) {
            disp_commit(pbuf, IMAGE_SIZE);
        }else {
            printf("error: %s, %d\n", __func__, __LINE__);
            break;
        }
    }

    free(pbuf);
    pbuf = NULL;
exit1:
    disp_exit();
exit2:
    rgbcamera_exit();
exit3:
    return NULL;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);

    pthread_t pid;
    if(0 != CreateNormalThread(displayCamera, NULL ,&pid)){
        return a.exec();
    }

    mainWidget w;
    w.show();

    return a.exec();
}
