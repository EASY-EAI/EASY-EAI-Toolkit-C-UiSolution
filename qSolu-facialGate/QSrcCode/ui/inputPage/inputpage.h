#ifndef INPUTPAGE_H
#define INPUTPAGE_H

#include <QFrame>
#include <QTimer>

namespace Ui {
class InputPage;
}

class InputPage : public QFrame
{
    Q_OBJECT

public:
    explicit InputPage(QWidget *parent = 0, bool bIsTimeOutInput = true);
    QString strContent(){ return m_strContent; }
    ~InputPage();

signals:
    void reSetCountDown(int swTimeOut);

private:
    void init();
    void pressNum(int num);

private slots:
    void on_mpBtn1_clicked();
    void on_mpBtn2_clicked();
    void on_mpBtn3_clicked();
    void on_mpBtn4_clicked();
    void on_mpBtn5_clicked();
    void on_mpBtn6_clicked();
    void on_mpBtn7_clicked();
    void on_mpBtn8_clicked();
    void on_mpBtn9_clicked();
    void on_mpBtn0_clicked();
    void on_mpBtnClear_clicked();
    void on_mpBtnEntry_clicked();
    void on_mpBtnRegister_clicked();

    void updateCountDown();
    void onReSetCountDown(int swTimeOut);

private:
    bool m_bIsTimeOutInput;
    int m_swCountDown;
    QTimer *m_pTimer;

    bool m_bIsFirstKey;

    QString m_strContent;
    Ui::InputPage *ui;
};

#endif // INPUTPAGE_H
