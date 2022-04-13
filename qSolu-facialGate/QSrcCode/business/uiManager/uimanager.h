#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QObject>
#include <QTimer>

#include "business/basemodel.h"

#include "ui/desktopview.h"
#include "ui/mainWidget.h"
#include "ui/mainwidgetevent.h"

class UIManager : public QObject, public BaseModel
{
    Q_OBJECT

public:
    explicit UIManager(QObject *parent = 0);
    ~UIManager();

    static UIManager *instance() { return m_pSelf; }
    static void createUIManager();

    /// handle >>>>: 处理其他模块过来输入的数据

    DesktopView *UIView_instance()    { return mpUIView;    }
    mainWidget  *MainWidget_instance(){ return mpMainWidget;}

    //  -------------- Get Data ---------------
    QRect UIViewRect(){ return m_UIViewRect; }

    //  -------------- Exe Cmd  ---------------
    void setSysPublicy(void *cpPublicy);
    void setSysTime(void *cpDateTime);
    int upDateSysTime();
    void setNotifyMsg(void *pNotify);

    int switchToStandBy();
    int switchToInput();
    int switchToNotify();

signals:
    void sendSysPublicy(std::string strPublicy);
    void sendSysTime(std::string strDateTime);
    void sendNotifyMsg(NotifyData_t data);

private:
    void Init();
    void registerMetaType();
    void setConnections();

private:
    static UIManager *m_pSelf;

    mainWidget *mpMainWidget;
    DesktopView *mpUIView;
    QRect m_UIViewRect;

    bool updateFace;

    pthread_t mTidManager;
};

#endif // UIMANAGER_H
