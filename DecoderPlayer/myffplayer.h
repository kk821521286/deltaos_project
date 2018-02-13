#ifndef MYFFPLAYER_H
#define MYFFPLAYER_H
//#include <qwindowdefs.h>

#include <QString>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
//#include <QImage>
#include <QTimer>
#include <QElapsedTimer>
#include<QDateTime>
//#include <QLabel>
#include "workerthread.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
};
class MyFfPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MyFfPlayer(QObject *parent = 0);
    ~MyFfPlayer();


    int timeoutCallback(void*args);
    void initShareMem();
    void flushListPacket();
    bool open(QString url, bool isLive = false);
    bool play();
    bool resume_play();
    bool stop();
    bool close();
    bool pause();
    bool isOpened();
    bool isPlaying();
    bool canPause();
    bool canSeek();
    int64_t position();
    long long length();
    long time();

    bool setPosition(int64_t position);
    bool setFullscreen(bool flag);
    void asyncReadPacket();
    static void readPacket(void* ctx);
    void asyncDecodePacket();
    static void decodePacket(void* ctx);

    void asyncReadAndDecodePacket();

    static void flushPacket(void *ctx);
    void asyncFlushPacket();

    int prepareDecodeInfo();


    static int interruptCallback(void *ctx);
    int internalInterruptCallback();
    int flag_pause;
    float time_base_num;
    float time_base_den;
    float mFileLength;

    unsigned char * pData; //共享内存指向的指针
signals:
protected:
    void timerEvent(QTimerEvent *);
private slots:
    void refreshVideo();
private:
    AVFormatContext* inputFormatCtx;
    int videoIndex;
    AVCodecContext* videoCodecCtx;
    AVCodec* videoCodec;
    AVFrame* videoFrame;
    AVFrame* videoFrameRGB;

    uint8_t* bufferRGB;
    struct SwsContext * swsCtxRGB;

    QThread* threadRead;
    bool quitRead;
    bool bQuit;

    int detectThreadRead;
    int detectThreadDecode;
    QThread* threadDecode;
    bool quitDecode;

    QList<AVPacket> listRead;
    QMutex* mutexListRead;
    QWaitCondition* condListRead;
    QWaitCondition* condListR;

    QMutex* mutexPause;
    QWaitCondition* condPause;

    QList<AVPicture> listDecoded;
    QMutex* mutexListDecoded;
    QWaitCondition* condListDecoded;

    QString url;
    bool bIsLive;
    bool opened;
    bool playing;
    int64_t pos;

    QTimer* refreshTimer;
    QElapsedTimer* refreshElapsedTimer;

    int64_t duration;
    int64_t pts;

    QThread* mThread;
    QThread* refreshThread;

    int flushTimer;
    int quitFlag;

    int _callbackTimes;
    int _testTimeout;
    QDateTime startTimeCallback;
    QDateTime endTimeCallback;

     AVPicture mPic;
};

#endif // MYFFPLAYER_H
