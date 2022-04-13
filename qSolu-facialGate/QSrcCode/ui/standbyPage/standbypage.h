#ifndef STANDBYPAGE_H
#define STANDBYPAGE_H

#include <QFrame>

namespace Ui {
class StandbyPage;
}

class StandbyPage : public QFrame
{
    Q_OBJECT

public:
    explicit StandbyPage(QWidget *parent = 0);
    ~StandbyPage();

private:
    Ui::StandbyPage *ui;
};

#endif // STANDBYPAGE_H
