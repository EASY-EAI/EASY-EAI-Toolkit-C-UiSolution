// ============================ Linux C ============================
#include "system.h"
// ========================== easyeai_api ==========================
#include "SystemOpt/SystemOpt.h"
// ============================ Common =============================
#include "wavPlayer/play_wav.h"
#include "wavPlayer/wavSplicer.h"
// ============================ Project ============================
#include "announcement.h"



Announcement *Announcement::m_pSelf = nullptr;
#define MUSIC_PLAY_TIME 65 //s
#define REGISTER_TIPS_TIME 15 //s

int AnnouncementCallback(void *data)
{
    /*此回调处理其他模块送入的消息*/
    int ret = -1;

    Announcement_t announcement;
    memcpy(&announcement, data, sizeof(DataBase_t));

    // handle
    if(MSG_ANMCMD_SETVOLUME == announcement.cmdType){
        ;
    }else if(MSG_ANMCMD_WELCOME == announcement.cmdType){
        Announcement::instance()->playWelcome();
    }else if(MSG_ANMCMD_REGISTSUCC == announcement.cmdType){
        Announcement::instance()->playRegisterSucc();
    }else if(MSG_ANMCMD_REGISTFAILD == announcement.cmdType){
        Announcement::instance()->playRegisterFaild();
    }else if(MSG_ANMCMD_PASS == announcement.cmdType){
        Announcement::instance()->playCompareSucc();
    }

    return ret;
}

Announcement::Announcement() :
    m_MusicPlaying(false),
    mMusicCountDown(0),
    m_RegisterTipsPlaying(false),
    mRegisterTipsCountDown(0),
    m_pTimer(nullptr)
{
    /*特别注意：不建议在构造函数内部使用跨模块调度(m_pScheduler->PosDataTo(mod,data)),目标业务模块有可能未被注册*/

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(musicIsStop()));
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(registerTipsIsStop()));
    m_pTimer->start(1000); //ms

    setVolume(5);
    play_wav_thread_init();

    playWelcome();
}

Announcement::~Announcement()
{
    play_wav_thread_exit();

    m_pTimer->stop();
    disconnect(m_pTimer, SIGNAL(timeout()), this, SLOT(musicIsStop()));
    disconnect(m_pTimer, SIGNAL(timeout()), this, SLOT(registerTipsIsStop()));
    delete m_pTimer;
}

void Announcement::createAnnouncement()
{
    if(m_pSelf == nullptr){
        once_flag oc;
        call_once(oc, [&] {
            m_pSelf = new Announcement;
        });
    }
}


void Announcement::playStopSport(void *data)
{
#if 0
    UI_SportInfo_t sportInfo;
    std::queue<std::string> wavFileList;

    memcpy(&sportInfo, data, sizeof(UI_SportInfo_t));

    wavFileList.push("./audio/sportSumTimes.wav");   //本次运动累计
    loadNumWav(sportInfo.times,wavFileList);
    wavFileList.push("./audio/ci.wav");

    wavFileList.push("./audio/sportDuringTime.wav");   //本次运动时间
    loadNumWav(sportInfo.time_hour,wavFileList);
    wavFileList.push("./audio/hour.wav");
    loadNumWav(sportInfo.time_min,wavFileList);
    wavFileList.push("./audio/minute.wav");
    loadNumWav(sportInfo.time_sec,wavFileList);
    wavFileList.push("./audio/second.wav");

    wavFileList.push("./audio/consumeCalorie.wav");   //消耗卡路里
    loadNumWav(sportInfo.Calorie_i,wavFileList);
    wavFileList.push("./audio/dot.wav");
    loadFloatWav(sportInfo.Calorie_f,wavFileList);
    wavFileList.push("./audio/qian.wav");
    wavFileList.push("./audio/Cal.wav");

    wavFileList.push("./audio/encourage.wav");

    wavSplicing("/tmp/output.wav",wavFileList);

    play_wav_stop();
    play_wav_signal("/tmp/output.wav");

    m_MusicPlaying = false;

    printf("sport end ...\n");
    return ;
#endif
}

void Announcement::playSportMusic(void *data)
{
    if(!m_MusicPlaying){
        printf("play music ....\n");
        reloadCountDown();
        play_wav_signal("./audio/sportMusic.wav");
        m_MusicPlaying = true;
    }else{
//        printf("do nothing ....\n");
    }
}

void Announcement::playRegisterTips()
{
    if(!m_RegisterTipsPlaying){
        printf("play music ....\n");
        reloadRegisterCountDown();
        play_wav_signal("./audio/weChatRegister.wav");
        m_RegisterTipsPlaying = true;
    }else{
//        printf("do nothing ....\n");
    }
    return ;
}

void Announcement::playWelcome()
{
    play_wav_signal(WELCOME_WAV);
    return ;
}

void Announcement::playRegisterSucc()
{
    play_wav_signal(REGISTER_SUCCESS_WAV);
    return ;
}

void Announcement::playRegisterFaild()
{
    play_wav_signal(REGISTER_LIMIT_WAV);
    return ;
}

void Announcement::playCompareSucc()
{
    play_wav_signal(RECOGNIZE_SUCC_WAV);
    return ;
}

void Announcement::musicIsStop()
{
    if(mMusicCountDown <= 0){
        mMusicCountDown = 0;
        m_MusicPlaying = false;
    }else{
        mMusicCountDown--;
    }
}

void Announcement::registerTipsIsStop()
{
    if(mRegisterTipsCountDown <= 0){
        mRegisterTipsCountDown = 0;
        m_RegisterTipsPlaying = false;
    }else{
        mRegisterTipsCountDown--;
    }
}

void Announcement::setVolume(int volume)
{
    char cmd[128] = {0};

    sprintf(cmd, "amixer cset name='Master Playback Volume' %d,%d", 85*volume, 85*volume);
    exec_cmd_by_system(cmd);
}

void Announcement::reloadCountDown()
{
    mMusicCountDown = MUSIC_PLAY_TIME;
}

void Announcement::reloadRegisterCountDown()
{
    mRegisterTipsCountDown = REGISTER_TIPS_TIME;
}

void Announcement::loadNumWav(int num, std::queue<string> &fileList)
{
    std::string tempFileName;

    int wanflag  = 0, wanNum  =0;
    int qianflag = 0, qianNum =0;
    int baiflag  = 0, baiNum  =0;
    int shiflag  = 0, shiNum  =0;

    wanNum = num%100000;
    wanNum /= 10000;
    if(0 < wanNum){
        wanflag = 1;
    }

    qianNum = num%10000;
    qianNum /= 1000;
    if(0 < qianNum){
        qianflag = 1;
    }

    baiNum = num%1000;
    baiNum /= 100;
    if(0 < baiNum){
        baiflag = 1;
    }

    shiNum = num%100;
    shiNum /= 10;
    if(0 < shiNum){
        shiflag = 1;
    }

    if(wanflag){
        tempFileName = getFilePath(wanNum);
        fileList.push(tempFileName);
        fileList.push("./audio/wan.wav");
        if(0 == qianflag)
            fileList.push("./audio/0.wav");
    }

    if(qianflag){
        tempFileName = getFilePath(qianNum);
        fileList.push(tempFileName);
        fileList.push("./audio/qian.wav");
        if(0 == baiflag)
            fileList.push("./audio/0.wav");
    }

    if(baiflag){
        tempFileName = getFilePath(baiNum);
        fileList.push(tempFileName);
        fileList.push("./audio/bai.wav");
        if(0 == shiflag)
            fileList.push("./audio/0.wav");
    }

    if(shiflag){
        tempFileName = getFilePath(shiNum);
        fileList.push(tempFileName);
        fileList.push("./audio/shi.wav");
    }

    if(0 <= num%10){
        tempFileName = getFilePath(num%10);
        fileList.push(tempFileName);
    }
}

void Announcement::loadFloatWav(int num, std::queue<string> &fileList)
{
    std::string tempFileName;

    int Num[5] = {0};
    int isValid = 0;

    Num[0] = num%100000;
    Num[0] /= 10000;

    Num[1] = num%10000;
    Num[1] /= 1000;

    Num[2] = num%1000;
    Num[2] /= 100;

    Num[3] = num%100;
    Num[3] /= 10;

    Num[4] = num%10;

    for(int i = 0; i < 5; i++)
    {
        if(!isValid){
            if(0 != Num[i]){
                isValid = 1;
            }
        }

        if(isValid){
            tempFileName = getFilePath(Num[i]);
            fileList.push(tempFileName);
        }
    }
    if(0 == num){
        tempFileName = getFilePath(num);
        fileList.push(tempFileName);
    }
}

char *Announcement::getFilePath(int num)
{
    switch (num) {
    case 0:
        return "./audio/0.wav";
        break;
    case 1:
        return "./audio/1.wav";
        break;
    case 2:
        return "./audio/2.wav";
        break;
    case 3:
        return "./audio/3.wav";
        break;
    case 4:
        return "./audio/4.wav";
        break;
    case 5:
        return "./audio/5.wav";
        break;
    case 6:
        return "./audio/6.wav";
        break;
    case 7:
        return "./audio/7.wav";
        break;
    case 8:
        return "./audio/8.wav";
        break;
    case 9:
        return "./audio/9.wav";
        break;
    default:
        return NULL;
        break;
    }
}
