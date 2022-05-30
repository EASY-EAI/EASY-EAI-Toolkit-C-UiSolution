#ifndef ANNOUNCEMENT_H
#define ANNOUNCEMENT_H

#include <string>
#include <queue>

#include "business/basemodel.h"

#include <QObject>
#include <QTimer>

class Announcement :  public QObject, public BaseModel
{
    Q_OBJECT
public:
    explicit Announcement();
    ~Announcement();

    static Announcement *instance()
    {
        if(m_pSelf == nullptr){
            once_flag oc;
            call_once(oc, [&] {
                m_pSelf = new Announcement;
            });
        }
        return m_pSelf;
    }
    static void createAnnouncement();

    /// handle >>>>: 处理其他模块过来输入的数据
    void playStopSport(void *data);
    void playSportMusic(void *data);
    void playRegisterTips();
    void playWelcome();
    void playRegisterSucc();
    void playRegisterFaild();
    void playCompareSucc();
public slots:
    void musicIsStop();
    void registerTipsIsStop();

private:
    void setVolume(int volume);

    void reloadCountDown(void);
    void reloadRegisterCountDown(void);

    void loadNumWav(int num, std::queue<std::string> &fileList);
    void loadFloatWav(int num, std::queue<std::string> &fileList);
    char *getFilePath(int num);

    // music
    bool m_MusicPlaying;
    int mMusicCountDown;
    // registerTips
    bool m_RegisterTipsPlaying;
    int mRegisterTipsCountDown;

    QTimer *m_pTimer;
    static Announcement *m_pSelf;
};


#endif // ANNOUNCEMENT_H
