#include "mainWidget.h"
#include "ui_mainWidget.h"

#include "standbyPage/standbypage.h"
#include "inputPage/inputpage.h"
#include "notifyPage/notifypage.h"

#include "system.h"
#include "SystemOpt/SystemOpt.h"

#include "Network/Network.h"

#include "uiManager/uimanager.h"

#include <QString>
#include <QList>
#include <QBitmap>
#include <QPixmap>
#include <QDebug>

using namespace std;

#define USE_PAGE_BY_CREATE

QString GBK2QString(char *pGbkCode)
{
    QByteArray array;
    QString strGbkCode = pGbkCode;
    for (int i=0; i<(strGbkCode.length() / 2); ++i)
    {
        uchar b = strGbkCode.mid(i*2, 2).toInt(nullptr, 16);
        array.append(b);
    }

    return QString::fromLocal8Bit(array);
}

mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    m_pTimer(nullptr),
    ui(new Ui::Widget)
{
    //设置为：取消窗口边框
    setWindowFlag(Qt::FramelessWindowHint, true);

    //设置为：背景透明
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background, Qt::transparent);
    setAutoFillBackground(true);
    setPalette(pal);

    ui->setupUi(this);
    init();
}

mainWidget::~mainWidget()
{
    m_pTimer->stop();
    disconnect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateDateTime()));
    delete m_pTimer;

#ifdef USE_PAGE_BY_CREATE
    int ret = -1;
    while(-1 == ret){
        ret = distroy_all_windows();
        usleep(5 * 1000);
    }
#else
    if(mpStandbyPage){
        delete mpStandbyPage;
        mpStandbyPage = nullptr;
    }
    if(mpInputPage){
        delete mpInputPage;
        mpInputPage = nullptr;
    }
    if(mpNotifyPage){
        delete mpNotifyPage;
        mpNotifyPage = nullptr;
    }
#endif

    delete ui;
}

void mainWidget::init()
{
    layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);//注意这里设置了所有参数页面的边框

//    ui->mpStandbyBtn->setIcon(QIcon(":/image/icon_batt_50.png"));
//    ui->mpStandbyBtn->setIcon(QIcon(":/image/icon_welcom_icon.png"));
//    QString str = "<html>按钮文字<br>多行显示</html>";
//    ui->mpStandbyBtn->setText(str);
//    uicontrol('Style', 'pushbutton', 'Units', 'normalized', 'FontSize', 12,...
//                   'position', [0.4 0.4 0.3 0.2], 'string', str);

    //left
    ui->mpDateLab->setStyleSheet("QLabel { font: 24px 'Ubuntu'; color: rgb(245, 245, 245); background-color: transparent; }");
    ui->mpWeekLab->setStyleSheet("QLabel { font: 36px 'Ubuntu'; color: rgb(245, 245, 245); background-color: transparent; }");
    ui->mpProductLab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");
    ui->mpSNLab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");

    //right
    ui->mpTimeLab->setStyleSheet("QLabel { font: 72px 'Ubuntu'; color: rgb(245, 245, 245); background-color: transparent; }");
    ui->mpIPLab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");
//    ui->mpWIFILab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateDateTime()));
    m_pTimer->start(1000);

    updateSysSNParam();
    updateSysIpParam();


    current_page = Page_NONE;
#ifdef USE_PAGE_BY_CREATE
    mpStandbyPage = new StandbyPage;
    layout->addWidget(mpStandbyPage);
    ui->mpMainPage->setLayout(layout);
#else
    mpStandbyPage = new StandbyPage;
    layout->addWidget(mpStandbyPage);
    mpInputPage = new InputPage(this, false);
    layout->addWidget(mpInputPage);
    mpNotifyPage = new NotifyPage;
    layout->addWidget(mpNotifyPage);
    switchToStandbyPage();
    ui->mpMainPage->setLayout(layout);
#endif
    current_page = Standby_Page;
}

int mainWidget::hide_all_windows()
{
    if(mpStandbyPage){
        mpStandbyPage->hide();
    }
    if(mpInputPage){
        mpInputPage->hide();
    }
    if(mpNotifyPage){
        mpNotifyPage->hide();
    }
    current_page = Page_NONE;

    return 0;
}

int mainWidget::distroy_all_windows()
{
    if (Standby_Page == current_page){
        delete mpStandbyPage;
        mpStandbyPage = NULL;
    } else if (Input_Page == current_page){
        delete mpInputPage;
        mpInputPage = NULL;
    } else if (Notify_Page == current_page){
        delete mpNotifyPage;
        mpNotifyPage = NULL;
    }
    current_page = Page_NONE;
    layout->takeAt(0);
    return 0;
}

void mainWidget::switchToStandbyPage()
{
#ifdef USE_PAGE_BY_CREATE
    if(-1 == distroy_all_windows()){
        return ;
    }

    mpStandbyPage = new StandbyPage;
    layout->addWidget(mpStandbyPage);
    ui->mpMainPage->setLayout(layout);

#else
    hide_all_windows();
    if(mpStandbyPage)
        mpStandbyPage->show();
#endif
    current_page = Standby_Page;
}

void mainWidget::switchToInputPage(bool bIsCounDownPage)
{
    struct timeval tv_be, tv_ed;
    qDebug("start to node time");
    gettimeofday(&tv_be, NULL);

#ifdef USE_PAGE_BY_CREATE
    if(-1 == distroy_all_windows()){
        return ;
    }

    mpInputPage = new InputPage(this, bIsCounDownPage);
    layout->addWidget(mpInputPage);
    ui->mpMainPage->setLayout(layout);

#else
    hide_all_windows();
    if(mpInputPage)
        mpInputPage->show();
#endif
    current_page = Input_Page;
    gettimeofday(&tv_ed, NULL);
    qDebug("use time : %ld", ((tv_ed.tv_usec - tv_be.tv_usec)+1000000)%1000000);
}

void mainWidget::switchToNotifyPage()
{
    if(Notify_Page == current_page)
        return ;

#ifdef USE_PAGE_BY_CREATE
    if(-1 == distroy_all_windows()){
        return ;
    }

    mpNotifyPage = new NotifyPage;
    layout->addWidget(mpNotifyPage);
    ui->mpMainPage->setLayout(layout);

    current_page = Notify_Page;

#else
    hide_all_windows();
    if(mpNotifyPage)
        mpNotifyPage->show();
#endif

    UIManager::instance()->SendDataToDataAnnouncement(MSG_ANMCMD_PASS, 0 ,NULL);
}

void mainWidget::getSysDateTime()
{
    uint32_t curDate = 0, curTime = 0;
    get_system_date_time(&curDate, &curTime);

    m_dateYear   = curDate/10000;
    m_dateMonth  = curDate%10000/100;
    m_dateDay    = curDate%100;
    m_dateWeek   = calc_week_day(m_dateYear, m_dateMonth, m_dateDay)+1;
    m_timeHour   = curTime/10000;
    m_timeMin    = curTime%10000/100;
    m_timeSecond = curTime%100;

}

void mainWidget::updateSysDateTime()
{
    QString strDate;
    strDate.clear();
    strDate.append(QString::number(m_dateYear));
    strDate.append(tr("年"));
    strDate.append(QString::number(m_dateMonth).asprintf("%02d",m_dateMonth));
    strDate.append(tr("月"));
    strDate.append(QString::number(m_dateDay).asprintf("%02d",m_dateDay));
    strDate.append(tr("日"));

    QString strWeek;
    strWeek.clear();
    switch (m_dateWeek) {
    case 1:
        strWeek.append("星期一");
        break;
    case 2:
        strWeek.append("星期二");
        break;
    case 3:
        strWeek.append("星期三");
        break;
    case 4:
        strWeek.append("星期四");
        break;
    case 5:
        strWeek.append("星期五");
        break;
    case 6:
        strWeek.append("星期六");
        break;
    case 7:
        strWeek.append("星期日");
        break;
    default:
        break;
    }

    QString strTime;
    strTime.clear();
    strTime.append(QString::number(m_timeHour).asprintf("%02d",m_timeHour));
    strTime.append(":");
    strTime.append(QString::number(m_timeMin).asprintf("%02d",m_timeMin));
//    strTime.append(":");
//    strTime.append(QString::number(m_timeSecond).sprintf("%02d",m_timeSecond));

//============================================================================================
    ui->mpDateLab->setText(strDate);
    ui->mpWeekLab->setText(strWeek);
    ui->mpTimeLab->setText(strTime);
}

void mainWidget::updateSysIpParam()
{
    printf("mainWidget::updateSysIpParam() ... \n");
    QString ipAddress;
    ipAddress.clear();

    char ip[16];
    msleep(200);    //读取ip前等到一段时间，避免未设好IP(需要一段等待时间)就马上读取的情况
    if(0 != get_local_Ip("eth0", ip, sizeof(ip)))
        return ;

    ipAddress.append(ip);
    ui->mpIPLab->setText(ipAddress);
}

void mainWidget::updateSysSNParam()
{
    QString cpuSN;
    cpuSN.clear();

    char SN[32] = {0};
    get_SN(SN, sizeof(SN));

    cpuSN.append("SN: ");
    cpuSN.append(SN);
    ui->mpSNLab->setText(cpuSN);
}

void mainWidget::on_mpStandbyBtn_clicked()
{
    if(Input_Page != current_page){
        switchToInputPage(true);
    }else{
        switchToStandbyPage();
    }
}

void mainWidget::onSetNotifyMsg(NotifyData_t data)
{
    if(mpNotifyPage){
        mpNotifyPage->setTipsColor(QString(data.textColor));
        mpNotifyPage->setUserHeadImg(QString(data.imagePath));
        mpNotifyPage->setUserId(QString(data.id));
        mpNotifyPage->setUserName(QString(data.name));
        mpNotifyPage->setTipsContent(QString(data.tips));
//        mpNotifyPage->setTipsContent(GBK2QString(data.tips));
        if(NULL != strstr(data.tips, "通知")){
            mpNotifyPage->setTipsBarBg(1);
        }else if(NULL != strstr(data.tips, "警告")){
            mpNotifyPage->setTipsBarBg(2);
        }else if(NULL != strstr(data.tips, "错误")){
            mpNotifyPage->setTipsBarBg(3);
        }else{
            mpNotifyPage->setTipsBarBg(0);
        }
//        mpNotifyPage->update();
    }
}


void mainWidget::onSetSNCode(string strSN)
{
    std::string strContent = "SN:";
    strContent += strSN;
//    printf("sn ------------- [ %s ]\n", strContent.c_str());
    ui->mpSNLab->setText(strContent.c_str());
    return ;
}

void mainWidget::onSetNetStatus(int connecting)
{
    if(connecting){
        ui->mpWIFILab->setStyleSheet("QLabel#mpWIFILab {image: url(:/image/icon_wifi_connect.png);}");
    }else{
        ui->mpWIFILab->setStyleSheet("QLabel#mpWIFILab {image: url(:/image/icon_wifi_disconnect.png);}");
    }
}

void mainWidget::onSetSysTime(string strDateTime)
{
    QString strYear  = QString::fromStdString(strDateTime.substr(0,2));
    QString strMonth = QString::fromStdString(strDateTime.substr(2,2));
    QString strDay   = QString::fromStdString(strDateTime.substr(4,2));
    QString strHour  = QString::fromStdString(strDateTime.substr(6,2));
    QString strMin   = QString::fromStdString(strDateTime.substr(8,2));
    QString strSec   = QString::fromStdString(strDateTime.substr(10,2));
    QString strWeek  = QString::fromStdString(strDateTime.substr(12,2));

    m_dateYear   = strYear.toInt();
    m_dateMonth  = strMonth.toInt();
    m_dateDay    = strDay.toInt();
    m_dateWeek   = strWeek.toInt();
    m_timeHour   = strHour.toInt();
    m_timeMin    = strMin.toInt();
    m_timeSecond = strSec.toInt();

    return ;
}

void mainWidget::onSetSysPublicy(string strPublicy)
{
    std::string strContent = strPublicy.substr(2, strPublicy.length() - 2);
    int lineNum = atoi(strPublicy.substr(0,2).c_str());
    if(1 == lineNum){
        ui->mpProductLab->setText(GBK2QString((char *)strContent.c_str()));
    }else if(2 == lineNum){
//        ui->mpSNLab->setText(GBK2QString(strContent.c_str()));
    }
    return ;
}

void mainWidget::customEvent(QEvent *e)
{
    if(MainWidgetEvent::MainWidgetEvent_SwitchToStandBy == (MainWidgetEvent::Type)e->type()){
        switchToStandbyPage();
    }else if(MainWidgetEvent::MainWidgetEvent_SwitchToInput == (MainWidgetEvent::Type)e->type()){
        switchToInputPage();
    }else if(MainWidgetEvent::MainWidgetEvent_SwitchToNotify == (MainWidgetEvent::Type)e->type()){
        switchToNotifyPage();
    }else if(MainWidgetEvent::MainWidgetEvent_UpdateSysTime == (MainWidgetEvent::Type)e->type()){
        updateSysDateTime();
    }else if(MainWidgetEvent::MainWidgetEvent_UpdateSysIP == (MainWidgetEvent::Type)e->type()){
        updateSysIpParam();
    }else if(MainWidgetEvent::MainWidgetEvent_EntryPassWord == (MainWidgetEvent::Type)e->type()){
        UIManager::instance()->SendDataToDataBase(MSG_DBCMD_SENDPASSWORD, mpInputPage->strContent().length(),
                                                  (void *)mpInputPage->strContent().toStdString().c_str());
        printf("passWord is ------ %s --------\n", mpInputPage->strContent().toStdString().c_str());
        switchToStandbyPage();
    }else if(MainWidgetEvent::MainWidgetEvent_Register == (MainWidgetEvent::Type)e->type()){
        int bAddRecord = true;
        UIManager::instance()->SendDataToMainThread(MSG_MTCMD_ADDRECORD, sizeof(int), &bAddRecord);
        switchToStandbyPage();
    }
}

void mainWidget::updateDateTime()
{
    getSysDateTime();
    updateSysDateTime();
}
