#include "system.h"
#include "uimanager.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDebug>

UIManager *UIManager::m_pSelf = NULL;

int UIManagerCallback(void *data)
{
    /*此回调处理其他模块送入的消息*/
    int ret = -1;

    UIManager_t UIMgr;
    memcpy(&UIMgr, data, sizeof(UIManager_t));

    // handle
    if(MSG_TAGPAGE_STANDBY == UIMgr.tagPage){
        switch (UIMgr.cmdType) {
        case MSG_UICMD_SHOW:
            ret = UIManager::instance()->switchToStandBy();
            break;
        case MSG_UICMD_UPDATEPUBLICY:
            ret = 0;
            break;
        case MSG_UICMD_UPDATETIME:
            if(!(0 < UIMgr.dataLen && NULL != UIMgr.data)){ return ret; }
            UIManager::instance()->setSysTime(UIMgr.data);
            ret = UIManager::instance()->upDateSysTime();
            break;
        default:
            break;
        }
    }else if(MSG_TAGPAGE_NOTIFY == UIMgr.tagPage){
        switch (UIMgr.cmdType) {
        case MSG_UICMD_SHOWTEXT:
            if(!(0 < UIMgr.dataLen && NULL != UIMgr.data)){ return ret; }
            ret = UIManager::instance()->switchToNotify();
            UIManager::instance()->setNotifyMsg(UIMgr.data);
            break;
        default:
            break;
        }
    }else if(MSG_TAGPAGE_INPUT == UIMgr.tagPage){
        switch (UIMgr.cmdType) {
        case MSG_UICMD_SHOWKEYBOARD:
            ret = UIManager::instance()->switchToInput();
            break;
        default:
            break;
        }
    }else{
        printf("invalid  tagPage ...\n");
    }

    return ret;
}

void *UIManagerThreadEntry(void *para)
{
    UIManager *pUIManager = (UIManager *)para;

    while(1)
    {
        if(NULL == pUIManager)
            break;


//        mSleep(1000);
    }

    return NULL;
}

UIManager::UIManager(QObject *parent) :
    QObject(parent),
    mpMainWidget(nullptr),
    mpUIView(nullptr)
{
    /*特别注意：不建议在构造函数内部使用跨模块调度(m_pScheduler->PosDataTo(mod,data)),目标业务模块有可能未被注册*/

    //嵌套关系：
    //  QGraphicsView           (mpUIView)
    //   ┗━━QGraphicsScene      (scene)
    //       ┗━━QWidget         (mainWidget)

    // 1 - 创建QGraphicsView
    mpUIView = new DesktopView();
    mpUIView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mpUIView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mpUIView->setAttribute(Qt::WA_AcceptTouchEvents, true);
//    mpUIView->setAttribute(Qt::WA_TranslucentBackground,true);
//    mpUIView->setAttribute(Qt::WA_StyledBackground);
    m_UIViewRect = QApplication::desktop()->availableGeometry();

    // 2 - 创建场景
    QGraphicsScene *scene = new QGraphicsScene(mpUIView);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    ///> 2.1-向场景中加入Item
//    mpVideoItem = new VideoItem(m_UIViewRect);
//    mpVideoItem->setZValue(0);
//    scene->addItem(mpVideoItem);
    ///> 2.2-向场景中加入Widget
    mpMainWidget = new mainWidget;
    scene->addWidget(mpMainWidget);

    scene->setSceneRect(scene->itemsBoundingRect());

    // 3 - 向 QGraphicsView 中加入 场景
    mpUIView->setScene(scene);
    mpUIView->show();
    Init();
}
UIManager::~UIManager()
{
    delete mpUIView;
    delete mpMainWidget;
    delete m_pSelf;
    m_pSelf = NULL;
}

void UIManager::createUIManager()
{
    if(m_pSelf == NULL)
        m_pSelf = new UIManager;
}

void UIManager::setSysPublicy(void *cpPublicy)
{
    if(NULL == cpPublicy)
        return ;

    if(mpMainWidget){
        std::string strPublicy = (char *)cpPublicy;
        emit sendSysPublicy(strPublicy);
//        mpMainWidget->setSysPublicy(strPublicy);
    }
}

void UIManager::setSysTime(void *cpDateTime)
{
    if(NULL == cpDateTime)
        return ;

    if(mpMainWidget){
        std::string strDateTime = (char *)cpDateTime;
        emit sendSysTime(strDateTime);
    }
}

int UIManager::upDateSysTime()
{
    int ret = -1;
    if(mpMainWidget){
        QEvent *ceEvent = new MainWidgetEvent(MainWidgetEvent::MainWidgetEvent_UpdateSysTime);
        QApplication::postEvent(mpMainWidget, ceEvent );  // Qt will delete it when done

        ret = 0;
    }

    return ret;
}

void UIManager::setNotifyMsg(void *pNotify)
{
    if(NULL == pNotify)
        return ;

    if(mpMainWidget){
        NotifyData_t notify_data;
        memcpy(&notify_data, pNotify, sizeof(NotifyData_t));
        emit sendNotifyMsg(notify_data);
    }
}

int UIManager::switchToStandBy()
{
    int ret = -1;
    if(mpMainWidget){
        QEvent *ceEvent = new MainWidgetEvent(MainWidgetEvent::MainWidgetEvent_SwitchToStandBy);
        QApplication::postEvent(mpMainWidget, ceEvent );  // Qt will delete it when done

        ret = 0;
    }

    return ret;
}

int UIManager::switchToInput()
{
    int ret = -1;
    if(mpMainWidget){
        QEvent *ceEvent = new MainWidgetEvent(MainWidgetEvent::MainWidgetEvent_SwitchToInput);
        QApplication::postEvent(mpMainWidget, ceEvent );  // Qt will delete it when done

        ret = 0;
    }

    return ret;
}

int UIManager::switchToNotify()
{
    int ret = -1;
    if(mpMainWidget){
        QEvent *ceEvent = new MainWidgetEvent(MainWidgetEvent::MainWidgetEvent_SwitchToNotify);
        QApplication::postEvent(mpMainWidget, ceEvent );  // Qt will delete it when done

        ret = 0;
    }

    return ret;
}

void UIManager::Init()
{
//    if(0 != createNormalThread(UIManagerThreadEntry, this, &mTidManager)){
//        free(pIPCS);
//    }
    registerMetaType();
    setConnections();
}
void UIManager::registerMetaType()
{
    qRegisterMetaType<NotifyData_t>("NotifyData_t");

    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<QVector<std::string>>("QVector<std::string>");

    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<QVector<QString> >("QVector<QString>");
}

void UIManager::setConnections()
{
    connect(this, &UIManager::sendSysPublicy, mpMainWidget, &mainWidget::onSetSysPublicy);
    connect(this, &UIManager::sendSysTime, mpMainWidget, &mainWidget::onSetSysTime);
    connect(this, &UIManager::sendNotifyMsg, mpMainWidget, &mainWidget::onSetNotifyMsg);
}
