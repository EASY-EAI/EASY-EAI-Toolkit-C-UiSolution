#include <QApplication>
#include <QTextCodec>

#include <opencv2/opencv.hpp>

#include "system.h"

#include "SystemOpt/SystemOpt.h"
#include "Display/Display.h"

#include "mainWidget.h"

#define	DISP_WIDTH		720
#define	DISP_HEIGHT		1280
void *displayScrean(void *para)
{
Q_UNUSED(para);
    disp_preset_uiLayer(SYS_TRUE);
    int ret = disp_init(DISP_WIDTH, DISP_HEIGHT); //RGB888 default
    if (ret) {
        printf("error func:%s, line:%d\n", __func__, __LINE__);
        return NULL;
    }

    cv::Mat image;
    image = cv::imread("background.jpg", 1);

    while(1) {
        if (!ret) {
            disp_commit(image.data, image.cols*image.rows*3);
        }else {
            printf("error: %s, %d\n", __func__, __LINE__);
            break;
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);

    pthread_t pid;
    if(0 != CreateNormalThread(displayScrean, NULL ,&pid)){
        return a.exec();
    }

    mainWidget w;
    w.show();

    return a.exec();
}
