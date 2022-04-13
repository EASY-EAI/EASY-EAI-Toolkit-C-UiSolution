#include "desktopview.h"

using namespace std;


DesktopView::DesktopView(QWidget *parent)
	: QGraphicsView(parent)
{
    setStyleSheet("background: transparent");
}

DesktopView::~DesktopView()
{

}
