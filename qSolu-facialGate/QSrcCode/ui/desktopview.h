#ifndef DESKTOPVIEW_H
#define DESKTOPVIEW_H

#include <QGraphicsView>

class DesktopView : public QGraphicsView
{
	Q_OBJECT

public:

    explicit DesktopView(QWidget *parent = 0);
	virtual ~DesktopView();

private:

};

#endif // DESKTOPVIEW_H
