#ifndef MAINWIDGETEVENT_H
#define MAINWIDGETEVENT_H


#include <QEvent>

class MainWidgetEvent:public QEvent
{
public:
    enum Type{
        MainWidgetEvent_None = QEvent::User,
        MainWidgetEvent_SwitchToStandBy,
        MainWidgetEvent_SwitchToInput,
        MainWidgetEvent_SwitchToNotify,
        MainWidgetEvent_EntryPassWord,
        MainWidgetEvent_Register,
        MainWidgetEvent_UpdateSysTime,
        MainWidgetEvent_UpdateSysIP,
        MainWidgetEvent_Num
    };

    MainWidgetEvent(MainWidgetEvent::Type type) : QEvent((QEvent::Type)type){ mMainMenuType = type; }
    ~MainWidgetEvent(){ mMainMenuType = MainWidgetEvent_None; }

    Type MainMenuType(){return mMainMenuType;}

private:
    Type mMainMenuType;
};
#endif // MAINWIDGETEVENT_H
