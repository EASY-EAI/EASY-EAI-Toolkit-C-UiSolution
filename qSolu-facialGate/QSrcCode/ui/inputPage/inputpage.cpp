#include "inputpage.h"
#include "ui_inputpage.h"

#include "business/uiManager/uimanager.h"

#define INPUT_MAX_COUNT 6

#define UNIT_TIME       (1000)          //ms
#define TIMER_TIMEOUT   (10*UNIT_TIME)  //ms

InputPage::InputPage(QWidget *parent, bool bIsTimeOutInput) :
    QFrame(parent),
    m_bIsTimeOutInput(bIsTimeOutInput),
    m_swCountDown(TIMER_TIMEOUT),
    m_pTimer(nullptr),
    m_bIsFirstKey(true),
    m_strContent("请输入密码"),
    ui(new Ui::InputPage)
{
    ui->setupUi(this);
    init();
}

InputPage::~InputPage()
{
    disconnect(this, SIGNAL(reSetCountDown(int)), this, SLOT(onReSetCountDown(int)));

    disconnect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateCountDown()));
    delete m_pTimer;

    delete ui;
}

void InputPage::init()
{
#if 0
    QString strClearBtnText(QApplication::translate("Widget",
                                                    "<html> <head/><body> \
                                                    <p><span style=\" font-size:20px; color:#ff0000; \">C</span></p> \
                                                    <p><span style=\" font-size:20px; color:#ffffff;  \">清除</span></p> \
                                                    <p><br/></p> \
                                                    </body></html>", nullptr));
    QString strEntryBtnText(QApplication::translate("Widget",
                                                    "<html> <head/><body> \
                                                    <p><span style=\" font-size:70px; font-weight:600; font-style:italic; \">E</span></p> \
                                                    <p><span style=\" font-size:20px; font-weight:600; font-style:italic; \">确认</span></p> \
                                                    </body></html>", nullptr));
#endif

    ui->mpBtn0->setIcon(QIcon(":/image/icon_num0.png"));
    ui->mpBtn1->setIcon(QIcon(":/image/icon_num1.png"));
    ui->mpBtn2->setIcon(QIcon(":/image/icon_num2.png"));
    ui->mpBtn3->setIcon(QIcon(":/image/icon_num3.png"));
    ui->mpBtn4->setIcon(QIcon(":/image/icon_num4.png"));
    ui->mpBtn5->setIcon(QIcon(":/image/icon_num5.png"));
    ui->mpBtn6->setIcon(QIcon(":/image/icon_num6.png"));
    ui->mpBtn7->setIcon(QIcon(":/image/icon_num7.png"));
    ui->mpBtn8->setIcon(QIcon(":/image/icon_num8.png"));
    ui->mpBtn9->setIcon(QIcon(":/image/icon_num9.png"));
//    ui->mpBtn9->setStyleSheet("QPushButton { font:25 36pt 'Ubuntu'; color: rgb(245, 245, 245); }");

    ui->mpTimeOutLab->setStyleSheet("QLabel { font: 36px 'Ubuntu'; color: rgb(245, 245, 245); }");
    ui->mpTimeOutLab->setHidden(true);

    ui->mpInputContent->setStyleSheet("QLabel { font: 36px 'Ubuntu'; color: rgb(245, 245, 245); }");
    ui->mpInputContent->setText(m_strContent);

    if(m_bIsTimeOutInput){
        m_pTimer = new QTimer(this);
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateCountDown()));

        connect(this, SIGNAL(reSetCountDown(int)), this, SLOT(onReSetCountDown(int)));
        emit reSetCountDown(TIMER_TIMEOUT);
        ui->mpTimeOutLab->setHidden(false);
    }
}

void InputPage::pressNum(int num)
{
    if(m_bIsFirstKey){
        m_bIsFirstKey = false;
        m_strContent.clear();
        m_strContent += QString::number(num);
    }else{
        if(m_strContent.count() >= INPUT_MAX_COUNT){
            return ;
        }

        m_strContent += QString::number(num);
    }

    ui->mpInputContent->setText(m_strContent);
}

void InputPage::on_mpBtn1_clicked()
{
    pressNum(1);
    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtn2_clicked()
{
    pressNum(2);
    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtn3_clicked()
{
    pressNum(3);
    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtn4_clicked()
{
    pressNum(4);
    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtn5_clicked()
{
    pressNum(5);
    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtn6_clicked()
{
    pressNum(6);
    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtn7_clicked()
{
    pressNum(7);
    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtn8_clicked()
{
    pressNum(8);
    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtn9_clicked()
{
    pressNum(9);
    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtnClear_clicked()
{
    m_bIsFirstKey = true;

    m_strContent.clear();
    m_strContent += QString("请输入密码");

    ui->mpInputContent->setText(m_strContent);

    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtn0_clicked()
{
    pressNum(0);
    emit reSetCountDown(TIMER_TIMEOUT);
}

void InputPage::on_mpBtnEntry_clicked()
{
    if(UIManager::instance()->MainWidget_instance()){
        QEvent *ceEvent = new MainWidgetEvent(MainWidgetEvent::MainWidgetEvent_EntryPassWord);
        QApplication::postEvent(UIManager::instance()->MainWidget_instance(), ceEvent );  // Qt will delete it when done
    }
    emit reSetCountDown(0);
}

void InputPage::on_mpBtnRegister_clicked()
{
    if(UIManager::instance()->MainWidget_instance()){
        QEvent *ceEvent = new MainWidgetEvent(MainWidgetEvent::MainWidgetEvent_Register);
        QApplication::postEvent(UIManager::instance()->MainWidget_instance(), ceEvent );  // Qt will delete it when done
    }
    emit reSetCountDown(0);
}


void InputPage::updateCountDown()
{
    m_swCountDown -= UNIT_TIME;
    if(m_swCountDown <= 0){
        m_pTimer->stop();

        //向上抛出事件，销毁输入页面
        if(UIManager::instance()->MainWidget_instance()){
            QEvent *ceEvent = new MainWidgetEvent(MainWidgetEvent::MainWidgetEvent_SwitchToStandBy);
            QApplication::postEvent(UIManager::instance()->MainWidget_instance(), ceEvent );  // Qt will delete it when done
        }
    }
    ui->mpTimeOutLab->setText(QString::number(m_swCountDown/UNIT_TIME));
}

void InputPage::onReSetCountDown(int swTimeOut)
{
    m_swCountDown = swTimeOut;
    m_pTimer->start(UNIT_TIME);
    ui->mpTimeOutLab->setText(QString::number(m_swCountDown/UNIT_TIME));
}
