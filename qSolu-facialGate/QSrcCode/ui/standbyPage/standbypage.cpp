#include "standbypage.h"
#include "ui_standbypage.h"

StandbyPage::StandbyPage(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::StandbyPage)
{
    ui->setupUi(this);
}

StandbyPage::~StandbyPage()
{
    delete ui;
}
