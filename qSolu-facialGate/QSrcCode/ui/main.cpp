#include <QApplication>
#include <QTextCodec>

#include "appscheduler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);

    AppScheduler App;

    return a.exec();
}
