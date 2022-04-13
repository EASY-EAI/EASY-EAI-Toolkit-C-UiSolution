#include "notifypage.h"
#include "ui_notifypage.h"

#include "uiManager/uimanager.h"

#include <QPainter>

#define UNIT_TIME       (1000)          //ms
#define TIMER_TIMEOUT   (10*UNIT_TIME)  //ms

NotifyPage::NotifyPage(QWidget *parent, bool bIsTimeOutInput) :
    QFrame(parent),
    m_bIsTimeOutInput(bIsTimeOutInput),
    m_swCountDown(TIMER_TIMEOUT),
    m_pTimer(nullptr),
    ui(new Ui::NotifyPage)
{
    ui->setupUi(this);

    if(m_bIsTimeOutInput){
        m_pTimer = new QTimer(this);
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateCountDown()));

        connect(this, SIGNAL(reSetCountDown(int)), this, SLOT(onReSetCountDown(int)));
        emit reSetCountDown(TIMER_TIMEOUT);
    }
}

NotifyPage::~NotifyPage()
{
    disconnect(this, SIGNAL(reSetCountDown(int)), this, SLOT(onReSetCountDown(int)));

    disconnect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateCountDown()));
    delete m_pTimer;

    delete ui;
}

void NotifyPage::setTipsBarBg(int index)
{
    switch (index) {
    case 1:
        ui->mpBgFrame->setStyleSheet("QFrame#mpBgFrame { border-image: url(:/image/img_tipsbar_bg_normal.png); }");
        break;
    case 2:
        ui->mpBgFrame->setStyleSheet("QFrame#mpBgFrame { border-image: url(:/image/img_tipsbar_bg_warning.png); }");
        break;
    case 3:
        ui->mpBgFrame->setStyleSheet("QFrame#mpBgFrame { border-image: url(:/image/img_tipsbar_bg_error.png); }");
        break;
    default:
        ui->mpBgFrame->setStyleSheet("QFrame#mpBgFrame { border-image: url(:/image/img_tipsbar_bg.png); }");
        break;
    }
}

void NotifyPage::setTipsColor(QString strColor)
{
    m_Color = strColor;
}

void NotifyPage::setTipsContent(QString strTips)
{
    m_TipsContent = strTips;
}

void NotifyPage::setUserName(QString strName)
{
    m_UserName = strName;
}

void NotifyPage::setUserId(QString strId)
{
    m_UserId = strId;
}

void NotifyPage::setUserHeadImg(QString strImgPath)
{
    m_HeadImg = strImgPath;
}

void NotifyPage::updateCountDown()
{
    m_swCountDown -= UNIT_TIME;
    if(m_swCountDown <= 0){
        m_pTimer->stop();

        //向上抛出事件，销毁通知页面
        if(UIManager::instance()->MainWidget_instance()){
            QEvent *ceEvent = new MainWidgetEvent(MainWidgetEvent::MainWidgetEvent_SwitchToStandBy);
            QApplication::postEvent(UIManager::instance()->MainWidget_instance(), ceEvent );  // Qt will delete it when done
        }
    }
}

void NotifyPage::onReSetCountDown(int swTimeOut)
{
    m_swCountDown = swTimeOut;
    m_pTimer->start(UNIT_TIME);
}

void NotifyPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QFont font;
    font.setPointSize(9);
    font.setWeight(70);
    painter.setFont(font);

    painter.setPen(QColor(255, 255, 0));

    QString color = "#" + m_Color;
    QPalette pe;
    pe.setColor(QPalette::WindowText, QColor(color.toStdString().c_str()));
    ui->mpNotifyLab->setPalette(pe);
    ui->mpNotifyLab->setText(m_TipsContent);
    ui->mpNameLab->setPalette(pe);
    ui->mpNameLab->setText(m_UserName);
    ui->mpIdLab->setPalette(pe);
    ui->mpIdLab->setText(m_UserId);

    // 加载头像路径
    QImage img;
    img.load(m_HeadImg);
    // 设置图片大小
    QImage imgscaled;
    imgscaled = img.scaled(192,192, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPixmap picPixmap = QPixmap::fromImage(imgscaled);
    ui->mpHeadImg->setPixmap(picPixmap);
}
