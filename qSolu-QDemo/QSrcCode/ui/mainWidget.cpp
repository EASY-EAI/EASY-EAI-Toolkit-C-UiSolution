
#include "mainWidget.h"
#include "ui_mainWidget.h"
#include "system.h"
#include "SystemOpt/SystemOpt.h"
#include "Network/Network.h"

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
    mpTextLab(NULL),
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

    delete ui;
}

void mainWidget::init()
{
    layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);//注意这里设置了所有参数页面的边框

    //left
    ui->mpDateLab->setStyleSheet("QLabel { font: 24px 'Ubuntu'; color: rgb(245, 245, 245); background-color: transparent; }");
    ui->mpWeekLab->setStyleSheet("QLabel { font: 36px 'Ubuntu'; color: rgb(245, 245, 245); background-color: transparent; }");
    ui->mpProductLab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");

    //right
    ui->mpTimeLab->setStyleSheet("QLabel { font: 72px 'Ubuntu'; color: rgb(245, 245, 245); background-color: transparent; }");
    ui->mpIPLab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");
//    ui->mpWIFILab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");

    getSysDateTime();
    updateSysDateTime();
    updateSysIpParam();

    mpTextLab = new QLabel;
    mpTextLab->setStyleSheet("QLabel { color: rgb(162, 195, 56); background-color: transparent; }");
    QFont font("Microsoft YaHei", 12, 87);
    mpTextLab->setFont(font);
    mpTextLab->setAlignment(Qt::AlignCenter);
    mpTextLab->setText(QString("欢迎使用easyeai_api"));
    layout->addWidget(mpTextLab);
    ui->mpMainPage->setLayout(layout);
}

void mainWidget::getSysDateTime()
{
    uint32_t curDate = 0, curTime = 0;
    get_system_date_time(&curDate, &curTime);

    m_dateYear   = curDate/10000;
    m_dateMonth  = curDate%10000/100;
    m_dateDay    = curDate%100;
    m_dateWeek   = calc_week_day(m_dateYear, m_dateMonth, m_dateDay);
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
    strDate.append(QString::number(m_dateMonth).sprintf("%02d",m_dateMonth));
    strDate.append(tr("月"));
    strDate.append(QString::number(m_dateDay).sprintf("%02d",m_dateDay));
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
    strTime.append(QString::number(m_timeHour).sprintf("%02d",m_timeHour));
    strTime.append(":");
    strTime.append(QString::number(m_timeMin).sprintf("%02d",m_timeMin));
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

void mainWidget::on_mpStandbyBtn_clicked()
{
    if(mpTextLab){
        if(mpTextLab->isHidden())
            mpTextLab->show();
        else
            mpTextLab->hide();
    }
}
