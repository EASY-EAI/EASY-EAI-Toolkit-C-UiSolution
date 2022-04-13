#ifndef NOTIFYPAGE_H
#define NOTIFYPAGE_H

#include <QFrame>
#include <QString>

namespace Ui {
class NotifyPage;
}

class NotifyPage : public QFrame
{
    Q_OBJECT

public:
    explicit NotifyPage(QWidget *parent = 0, bool bIsTimeOutInput = true);
    ~NotifyPage();

    void setTipsBarBg(int index);
    void setTipsColor(QString strColor);
    void setTipsContent(QString strTips);
    void setUserName(QString strName);
    void setUserId(QString strId);
    void setUserHeadImg(QString strImgPath);

signals:
    void reSetCountDown(int swTimeOut);
private slots:
    void updateCountDown();
    void onReSetCountDown(int swTimeOut);

private:
    bool m_bIsTimeOutInput;
    int m_swCountDown;
    QTimer *m_pTimer;

    void paintEvent(QPaintEvent *);

    Ui::NotifyPage *ui;
    QString m_Color;
    QString m_TipsContent;
    QString m_UserName;
    QString m_UserId;
    QString m_HeadImg;
};

#endif // NOTIFYPAGE_H
