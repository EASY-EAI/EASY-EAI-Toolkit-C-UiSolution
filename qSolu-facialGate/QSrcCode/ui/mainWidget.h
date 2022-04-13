#ifndef __MAINWIDGET_H__
#define __MAINWIDGET_H__

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QTimer>

#include "bsprotocol.h"
#include "mainwidgetevent.h"

class QHBoxLayout;

class StandbyPage;
class InputPage;
class NotifyPage;

namespace Ui {
class Widget;
}

enum Page_Type{
    Page_NONE = 0,
    Standby_Page,
    Input_Page,
    Notify_Page
};

class mainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit mainWidget(QWidget *parent = 0);
    ~mainWidget();

public slots:
    void on_mpStandbyBtn_clicked();
    void onSetNotifyMsg(NotifyData_t data);
    void onSetSNCode(std::string strSN);
    void onSetNetStatus(int connecting);
    void onSetSysTime(std::string strDateTime);
    void onSetSysPublicy(std::string strPublicy);

private slots:
    void updateDateTime();

private:
    void init();

    // Pages
    int hide_all_windows();
    int distroy_all_windows();
    void switchToStandbyPage();
    void switchToInputPage(bool bIsCounDownPage = false);
    void switchToNotifyPage();
    // mainWidget
    void getSysDateTime();
    void updateSysDateTime();
    void updateSysIpParam();
    void updateSysSNParam();

    void customEvent(QEvent *e);

    // Pages Manager
    QHBoxLayout *layout;
    int current_page;
    StandbyPage *mpStandbyPage;
    InputPage *mpInputPage;
    NotifyPage *mpNotifyPage;
    // mainWidget
    int m_dateYear;
    int m_dateMonth;
    int m_dateDay;
    int m_dateWeek;
    int m_timeHour;
    int m_timeMin;
    int m_timeSecond;

    QTimer *m_pTimer;

    Ui::Widget *ui;
};

#endif // WIDGET_H
