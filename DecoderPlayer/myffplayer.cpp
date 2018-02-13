#include "myffplayer.h"
#include "debug.h"
#include <QCoreApplication>
#include <QDebug>
//#include <windows.h>
#include <vxWorks.h>
#include <stdlib.h>
#include <semLib.h>
#include <rtpLib.h>
#include <taskLib.h>
#include <msgQLib.h>
#include <errnoLib.h>
#include <tickLib.h>
#include <tickLib.h>

#include <sys/mman.h>
#include<semaphore.h>
#include <unistd.h>

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

MSG_Q_ID msgId;

SEM_ID semIdShare;
int g_dbg_level = 0;
FILE *g_log_fp = NULL;


#define WIDTH 1600
#define HIGH 1200


static int timeoutCallbackStatic(void *ptr)
{
      MyFfPlayer* pMyPlayer = (MyFfPlayer*)ptr;
      return pMyPlayer->timeoutCallback(NULL);
}
int MyFfPlayer::timeoutCallback(void*args)
{
   // qDebug("timeoutCallback--0");
    if(_callbackTimes == 0){
        _callbackTimes = 1;
        return 0;
    }else{
        endTimeCallback = QDateTime::currentDateTime();
        int value = endTimeCallback.toMSecsSinceEpoch()/1000 - startTimeCallback.toMSecsSinceEpoch()/1000;
//        qDebug("timeoutCallback--1");
        if(value > _testTimeout){
            qDebug("timeoutCallback--2");
            return 1;
        }else{
            return 0;
        }
    }
    return 0;
}
MyFfPlayer::MyFfPlayer(QObject* parent) :
    QObject(parent)
{
    av_register_all();
    avcodec_register_all();
    avformat_network_init();
   // av_log_set_level(AV_LOG_DEBUG);

    playing = false;
    opened = false;
    bIsLive = false;
    inputFormatCtx = NULL;
    url = "";
    videoIndex = 0;
    videoCodecCtx = NULL;
    videoCodec =NULL;
    videoFrame = NULL;
    videoFrameRGB = NULL;
    bufferRGB = NULL;
    swsCtxRGB = NULL;
    threadRead = NULL;
    quitRead = false;
    mutexPause = NULL;
    time_base_num = 0;
    time_base_den = 0;

    initShareMem();

    mutexListRead = new QMutex;
    mutexListDecoded = new QMutex;
    condListRead = new QWaitCondition;
    condListDecoded = new QWaitCondition;
    condPause = new QWaitCondition;
    flag_pause = 0;
    bQuit = false;
    quitFlag = 0;
    _testTimeout = 1;
    _callbackTimes = 0;



    qDebug()<<"ffplayer"<<this->thread();

}

MyFfPlayer::~MyFfPlayer(){

}

typedef struct mymsg{
    AVPacket pkt;
}MYMSG;

void MyFfPlayer::initShareMem()
{
    g_dbg_level = 15;
    g_log_fp = fopen("ffdecoder.log","a+");
    if (g_log_fp == NULL)
    {
        printf("open log file error\n");
    }


    semIdShare = semOpen("/mysem",SEM_TYPE_MUTEX,0,0,OM_DELETE_ON_LAST_CLOSE,(void*)"sem_msg");
    if(semIdShare == NULL){
        qWarning("create sem failed");
    }
    msgId = msgQCreate(1024,sizeof(MYMSG),MSG_Q_FIFO);
//     semIdShare = sem_open("/mysem",O_CREAT,0666,1);
//     if(semIdShare == SEM_FAILED){
//        qWarning("create sem failed");
//     }
     int fd;
     /* create a new SHM object */
     fd = shm_open("/myshm", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
     if (fd == -1)
     exit (1);
     /* set object size */
     long memSize = 64+WIDTH*HIGH*4;
     if (ftruncate (fd, memSize) == -1)
     exit (1);
     /* Map shared memory object in the address space of the process */
     pData = (unsigned char *) mmap (0, memSize, PROT_READ | PROT_WRITE,
     MAP_SHARED, fd, 0);
     if (pData == (unsigned char *) MAP_FAILED){
         qWarning("share memory malloc failed");
        exit (1);
     }
}
void MyFfPlayer::refreshVideo()
{
    if(quitFlag == 1){
        qDebug("refreshVideo -----3");
        return ;
    }
     quitFlag = 1;

  //  qDebug("refreshVideo ---0  listsize %d ",listDecoded.size());
    {

        mutexListDecoded->lock();
        if(listDecoded.isEmpty()){
            qWarning("list is empty %d!",(int)QThread::currentThreadId());
			mutexListDecoded->unlock();
            quitFlag = 0;
            return;
		}
        AVPicture pic = listDecoded.first();
        listDecoded.pop_front();
        mutexListDecoded->unlock();

     //   qDebug("refreshVideo ---1  listsize %d ",listDecoded.size());
        semTake (semIdShare,WAIT_FOREVER);
        memcpy(pData,(uchar*)pic.data[0],WIDTH*HIGH*4);
        semGive(semIdShare);

        avpicture_free(&pic);
    }
   // qDebug("refreshVideo end");
    quitFlag = 0;
}
void MyFfPlayer::timerEvent(QTimerEvent *e)
{
  if(e->type() == flushTimer){
      qDebug("flushTimer is aLive--0");
      refreshVideo();
      qDebug("flushTimer is aLive--1");
  }
}
bool MyFfPlayer::open(QString url,bool isLive){
    this->url = url;
    bIsLive = isLive;

/*
    mThread = new QThread(this);
    mThread->start();

    refreshTimer = new QTimer(0);

    if(bIsLive){
        qDebug("start refreshTimer .");
        refreshTimer->setInterval(40);
    }else{
        qDebug("start refreshTimer .");
        refreshTimer->setInterval(40);
    }
    refreshTimer->moveToThread(mThread);
    connect(refreshTimer,SIGNAL(timeout()),this,SLOT(refreshVideo()),Qt::DirectConnection);
    connect(mThread,SIGNAL(started()),refreshTimer,SLOT(start()));
*/
    bQuit = false;
//    if(bIsLive){
//        flushTimer = startTimer(40);
//    }else{
//        flushTimer = startTimer(40);
//    }
    refreshThread = new WorkerThread(this,MyFfPlayer::flushPacket);
    refreshThread->setStackSize(1*1024*1024);
    //refreshThread->start();

    flushListPacket();

    threadRead = new WorkerThread(this,MyFfPlayer::readPacket);
    threadRead->setStackSize(2*1024*1024);
    threadRead->start();

    qDebug("start read thread.");

    return true;
}
void MyFfPlayer::flushListPacket()
{
   while(listDecoded.size() > 0){
     AVPicture pic = listDecoded.front();
     listDecoded.pop_front();
     avpicture_free(&pic);
   }
   while(listRead.size() > 0){
     AVPacket pkt = listRead.front();
     listRead.pop_front();
     av_free_packet(&pkt);
   }
}
void MyFfPlayer::readPacket(void * ctx){
    MyFfPlayer* player = (MyFfPlayer*)ctx;
    player->asyncReadPacket();
}
void MyFfPlayer::decodePacket(void * ctx){
    MyFfPlayer* player = (MyFfPlayer*)ctx;
    player->asyncDecodePacket();
//    player->asyncReadAndDecodePacket();
}
void MyFfPlayer::flushPacket(void *ctx){
    MyFfPlayer* player = (MyFfPlayer*)ctx;
    player->asyncFlushPacket();
}
void MyFfPlayer::asyncFlushPacket()
{
    while(!bQuit)
    {
        qDebug("flush packaget ---------0");
        if(mutexListDecoded->tryLock()){
//            qDebug("flush packaget ---------3");
            if(listDecoded.isEmpty()){
                qDebug("flush packaget ---------1");
                mutexListDecoded->unlock();
                {
//                    struct timespec tv;
//                    struct timespec tv_rval;
//                    tv.tv_sec = 0;
//                    tv.tv_nsec = 20000000;
//                    qDebug("flush packaget ---------1.5");
//                    nanosleep(&tv,&tv_rval);
                    sleep(20);

                }
//                qWarning("list is empty %d!",(int)QThread::currentThreadId());
                qDebug("flush packaget ---------1.8");
                continue;
            }
            qDebug("flush packaget ---------3");
            AVPicture pic = listDecoded.first();
            listDecoded.pop_front();
            mutexListDecoded->unlock();
            qDebug()<<"flush packaget ----1=";
//            qDebug("flush packaget ---------1,listDecoded size %d",listDecoded.size());
            semTake (semIdShare,WAIT_FOREVER);
            memcpy(pData,(uchar*)pic.data[0],WIDTH*HIGH*4);
            semGive(semIdShare);
            qDebug()<<"flush packaget -----2";
//            qDebug("flush packaget ---------2,listDecoded size %d",listDecoded.size());
            avpicture_free(&pic);
            qDebug()<<"flush packaget --2.5";
        }else{
            qDebug("flush packaget ---------4");
        }
        {
//            struct timespec tv;
//            struct timespec tv_rval;
//            tv.tv_sec = 0;
//            tv.tv_nsec = 20000000;
//            qDebug("flush packaget ---------5,---%ld",tv.tv_nsec);
//            nanosleep(&tv,&tv_rval);
//            qDebug("flush packaget ---------5.5");
              sleep(20);
        }
    }
}
int MyFfPlayer::interruptCallback(void *ctx)
{
    MyFfPlayer* player = (MyFfPlayer*)ctx;
    return player->internalInterruptCallback();
}
int MyFfPlayer::internalInterruptCallback(){
    if(quitRead){
        return -1;
    }
    return 0;
}
int MyFfPlayer::prepareDecodeInfo()
{
    inputFormatCtx = avformat_alloc_context();

    AVDictionary * pic = NULL;
    av_dict_set(&pic,"rtsp_transport","tcp",0);
    QByteArray ba = url.toLatin1();
    qDebug("bytearray %s, %p",ba.data(), inputFormatCtx);
    if(avformat_open_input(&inputFormatCtx ,ba.data(),NULL,&pic)!=0){
        qWarning("Couldn't open input stream %s.\n",ba.data());
        goto err_exit;
    }
    if(avformat_find_stream_info(inputFormatCtx,NULL)<0)
    {
        qWarning("Couldn't find stream information.（无法获取流信息）\n");
        goto err_exit;
    }
    videoIndex = -1;
    for(int i=0; i<(int)inputFormatCtx->nb_streams; i++){
        if(inputFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoIndex=i;
            duration = inputFormatCtx->streams[i]->duration;
            time_base_num = inputFormatCtx->streams[i]->time_base.num;
            time_base_den = inputFormatCtx->streams[i]->time_base.den;
            mFileLength = duration*time_base_num/time_base_den;
            break;
        }
    }
    if(bIsLive){
        mFileLength = 0;
    }
    if(videoIndex == -1)
    {
        qWarning("Didn't find a video stream.\n");
    }else{
        videoCodecCtx=inputFormatCtx->streams[videoIndex]->codec;
        videoCodec=avcodec_find_decoder(videoCodecCtx->codec_id);
        if(videoCodec!=NULL)
        {
            if(avcodec_open2(videoCodecCtx, videoCodec,NULL)<0)
            {
                qWarning("Could not open video codec.\n");
                videoIndex = -1;
            }
        }else{
            videoIndex = -1;
        }
    }
    videoFrame = avcodec_alloc_frame();

    swsCtxRGB = sws_getContext(videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt,
                               videoCodecCtx->width, videoCodecCtx->height, PIX_FMT_RGB32,
                               SWS_BICUBIC, NULL, NULL, NULL);

   bQuit = false;
   playing = true;
   return 0;
err_exit:
    opened = false;

    if(inputFormatCtx){
        avformat_close_input(&inputFormatCtx);
        avformat_free_context(inputFormatCtx);
    }
    qWarning("Exit Read Proc %p\n", this);
    return -1;
}
void MyFfPlayer::asyncReadAndDecodePacket()
{
    if(-1 == prepareDecodeInfo()){
        qDebug("prepareDecodeInfo error");
        return;
    }
    while (!bQuit)
    {

        while(!bIsLive&&listDecoded.size() >= 5 && !bQuit){
            struct timespec tv;
            struct timespec tv_rval;
            tv.tv_sec = 0;
            tv.tv_nsec = 10000000;
            qDebug("nanoSleep");
            nanosleep(&tv,&tv_rval);
            continue;
        }

        AVPacket pkt;
        if(flag_pause == 0){

            if(av_read_frame(inputFormatCtx,&pkt)<0){
                qWarning("av_read_frame error !\n");
                break;
            }

        }else{
            continue;
        }
        av_dup_packet(&pkt);
        if(pkt.stream_index != videoIndex){
            av_free_packet(&pkt);
            continue;
        }


        int got_frame = 0;
        avcodec_decode_video2(videoCodecCtx,videoFrame,&got_frame,&pkt);
        if(got_frame){
            AVPicture pic;
            uint8_t* buffer=(uint8_t *)av_malloc(
                        avpicture_get_size(PIX_FMT_RGB32,
                                           videoCodecCtx->width,videoCodecCtx->height));
            avpicture_fill(&pic, buffer, PIX_FMT_RGB32,
                           videoCodecCtx->width,videoCodecCtx->height);

            sws_scale(swsCtxRGB,(const uint8_t* const *)videoFrame->data,videoFrame->linesize,
                      0,videoCodecCtx->height,pic.data,pic.linesize);
           // qDebug("decode packaget ---0");
            mutexListDecoded->lock();
            listDecoded.append(pic);
            pts = pkt.pts*time_base_num/time_base_den;
            mutexListDecoded->unlock();

           // qDebug("decode packaget ---1");

            qDebug("current process %lld",pts);
        }else{
            qDebug("decode packaget ---2");
        }
        av_free_packet(&pkt);
    }
}
void MyFfPlayer::asyncReadPacket(){

    inputFormatCtx = avformat_alloc_context();
    _callbackTimes = 1;
    AVIOInterruptCB intCB = {timeoutCallbackStatic,this};
    inputFormatCtx->interrupt_callback = intCB;
    AVDictionary * pic = NULL;
    av_dict_set(&pic,"rtsp_transport","tcp",0);
    QByteArray ba = url.toLatin1();
    startTimeCallback = QDateTime::currentDateTime();
    _testTimeout = 1;
    qDebug("bytearray %s, %p",ba.data(), inputFormatCtx);
    if(avformat_open_input(&inputFormatCtx ,ba.data(),NULL,&pic)!=0){
        qWarning("Couldn't open input stream %s.\n",ba.data());
        goto err_exit;
    }
    startTimeCallback = QDateTime::currentDateTime();
    if(avformat_find_stream_info(inputFormatCtx,NULL)<0)
    {
        qWarning("Couldn't find stream information.（无法获取流信息）\n");
        goto err_exit;
    }
    videoIndex = -1;
    for(int i=0; i<(int)inputFormatCtx->nb_streams; i++){
        if(inputFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoIndex=i;
            duration = inputFormatCtx->streams[i]->duration;
            time_base_num = inputFormatCtx->streams[i]->time_base.num;
            time_base_den = inputFormatCtx->streams[i]->time_base.den;
            mFileLength = duration*time_base_num/time_base_den;
            break;
        }
    }
    if(bIsLive){
        mFileLength = 0;
    }
    if(videoIndex == -1)
    {
        qWarning("Didn't find a video stream.\n");
    }else{
        videoCodecCtx=inputFormatCtx->streams[videoIndex]->codec;
        videoCodec=avcodec_find_decoder(videoCodecCtx->codec_id);
        if(videoCodec!=NULL)
        {
            if(avcodec_open2(videoCodecCtx, videoCodec,NULL)<0)
            {
                qWarning("Could not open video codec.\n");
                videoIndex = -1;
            }
        }else{
            videoIndex = -1;
        }
    }
    videoFrame = avcodec_alloc_frame();

    swsCtxRGB = sws_getContext(videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt,
                               videoCodecCtx->width, videoCodecCtx->height, PIX_FMT_RGB32,
                               SWS_BICUBIC, NULL, NULL, NULL);

    quitRead = false;
    quitDecode = false;

    threadDecode = new WorkerThread(this,MyFfPlayer::decodePacket);
    threadDecode->setStackSize(1*1024*1024);
    threadDecode->start();

    opened = true;
    playing = true;


    while (!bQuit)
    {

        AVPacket pkt;

        if(!bIsLive&&listRead.size()>10&&!bQuit){
//            qWarning("listRead size > 10");
//            struct timespec tv;
//            struct timespec tv_rval;
//            tv.tv_sec = 0;
//            tv.tv_nsec = 40000000;
//            nanosleep(&tv,&tv_rval);
            sleep(20);
            continue;
        }
        if(flag_pause == 0){
             startTimeCallback = QDateTime::currentDateTime();
             qDebug()<<"read frame start";
            if(av_read_frame(inputFormatCtx,&pkt)<0){
                qWarning("av_read_frame error !\n");
                sleep(20);
                continue;
            }
            qDebug()<<"read frame end";

        }else{
            qDebug()<<"flag_pause==1";
            continue;
        }
        av_dup_packet(&pkt);
        if(pkt.stream_index != videoIndex){
           qDebug()<<"stream_index wrong";
            av_free_packet(&pkt);
            continue;
        }
        MYMSG msg;
        msg.pkt = pkt;
        msgQSend(msgId,(char*)&msg,sizeof(MYMSG),WAIT_FOREVER,MSG_PRI_NORMAL);
        /*
        if(mutexListRead->tryLock(10)==true){
            if(flag_pause == 0){
                listRead.push_back(pkt);
            }
            qDebug("read frame list_size=%d",listRead.size());
            mutexListRead->unlock();
            condListRead->wakeOne();
        }else{
            qDebug("read frame mutexListRead trylock failed");
        }
        */
      //  qDebug("current read %lld listRead %ld",pkt.pts,listRead.size());
    }
     qDebug("readPacket EXIT");
    return;
err_exit:
    opened = false;
    if(threadDecode){
        quitDecode = true;
        threadDecode->wait();
        delete threadDecode;
        threadDecode = NULL;
        qWarning("threadDecode over!\n");
    }

    if(videoIndex>=0){
        avcodec_close(videoCodecCtx);
        videoCodecCtx = NULL;
        videoIndex = -1;
    }
    if(swsCtxRGB){
        sws_freeContext(swsCtxRGB);
        swsCtxRGB = NULL;
    }
    if(inputFormatCtx){
        avformat_close_input(&inputFormatCtx);
        avformat_free_context(inputFormatCtx);
    }
    qWarning("Exit Read Proc %p\n", this);
}
void MyFfPlayer::asyncDecodePacket(){

    uint8_t* buffer=(uint8_t *)av_malloc(
                avpicture_get_size(PIX_FMT_RGB32,
                                   videoCodecCtx->width,videoCodecCtx->height));
    avpicture_fill(&mPic, buffer, PIX_FMT_RGB32,
                   videoCodecCtx->width,videoCodecCtx->height);
    while(!bQuit){
        AVPacket pkt;
        qDebug("decode start ---------1");
        int flag = 0;
/*
        if(!bQuit&&listRead.size()==0){
            struct timespec tv;
            struct timespec tv_rval;
            tv.tv_sec = 0;
            tv.tv_nsec = 100000000;
            //qDebug("--decode---readsize 0");

            //nanosleep(&tv,&tv_rval);

            condListRead->wait(mutexListRead,10);
            //qDebug("--decode---readsize 0.5");
            //qDebug("-------listRead size %d",listRead.size());
            //condListRead->wait(mutexListRead);
            //qDebug("-------1---listRead size %d",listRead.size());
        }
        qDebug()<<"decode lockread";
        mutexListRead->lock();

        if(listRead.size()>0){
            pkt = listRead.front();
            listRead.pop_front();
            flag = 1;
        }
        mutexListRead->unlock();


        qDebug()<<"decode lockread end";
        if(flag==0){
            qDebug()<<"decode lockread no lock";
            continue;
        }
        */
        MYMSG msg;
        msgQReceive(msgId,(char*)&msg,sizeof(MYMSG),WAIT_FOREVER);
        qDebug()<<"decode recieved pkt";
        pkt = msg.pkt;
        int got_frame = 0;
        avcodec_decode_video2(videoCodecCtx,videoFrame,&got_frame,&pkt);
        qDebug()<<"decode  avcodec_decode_video2 finish";
        if(got_frame){


            sws_scale(swsCtxRGB,(const uint8_t* const *)videoFrame->data,videoFrame->linesize,
                      0,videoCodecCtx->height,mPic.data,mPic.linesize);

            qDebug()<<"decode lockdecode";
            //mutexListDecoded->lock();
            //listDecoded.append(pic);
            pts = pkt.pts*time_base_num/time_base_den;
            //mutexListDecoded->unlock();

            if(semTake (semIdShare,10)==0){
                memcpy(pData,(uchar*)mPic.data[0],WIDTH*HIGH*4);
                semGive(semIdShare);
            }


            qDebug()<<"decode lockdecode end";
           // qDebug("current process %lld",pts);
        }
        av_free_packet(&pkt);
        qDebug("decode end-----------");

    }
    avpicture_free(&mPic);
    qDebug("decodePacket EXIT");

}

bool MyFfPlayer::play(){

    if(opened){
        playing = true;
        if(flag_pause == 1){
            flag_pause = 0;
        }
    }else{
        playing = false;
    }
    return playing;
}
bool MyFfPlayer::resume_play(){
    if(opened&&playing == false){
        if(flag_pause == 1){
            flag_pause = 0;
        }
        if(inputFormatCtx){
            av_read_play(inputFormatCtx);
        }
        playing = true;
    }
    return playing;
}

long long MyFfPlayer::length(){

    if(isOpened()){
        return duration*time_base_num/time_base_den;
    }
    return 0;
}
long MyFfPlayer::time(){
    long ret = -1;

    if(isOpened()){
        /*
        if(inputFormatCtx){
            for(int i=0; i<inputFormatCtx->nb_streams; i++){
                if(inputFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
                {
                    //pts = inputFormatCtx->streams[i]->pts.val+inputFormatCtx->streams[i]->pts.num/inputFormatCtx->streams[i]->pts.den;
                    pts = inputFormatCtx->streams[i]->cur_dts*time_base_num/time_base_den;
                    return pts;
                }
            }
        }*/
        return pts;
    }
    return ret;
}

bool MyFfPlayer::stop(){
    playing = false;

    return opened;
}

bool MyFfPlayer::close(){
    opened = false;
    playing = false;
    bQuit = true;

    while(threadDecode->isRunning()){
        QCoreApplication::processEvents();
        threadDecode->wait();
    }
    delete threadDecode;
    threadDecode = NULL;
    qDebug("close() decode thread");
    while(threadRead->isRunning()){
        QCoreApplication::processEvents();
        threadRead->wait();
    }
    delete threadRead;
    threadRead = NULL;
    qDebug("close() refresh thread");
//    if(flushTimer != 0){
//        killTimer(flushTimer);
//    }
    while(refreshThread->isRunning()){
        QCoreApplication::processEvents();
        refreshThread->wait();
    }
    delete refreshThread;
    refreshThread = NULL;
    if(inputFormatCtx){
        avcodec_close(videoCodecCtx);
        qDebug("close() free videoCodecCtx");
        avcodec_free_frame(&videoFrame);
        qDebug("close() free videoFrame");
        videoFrame = NULL;
//        av_close_input_file(inputFormatCtx);
        avformat_close_input(&inputFormatCtx);
        avformat_free_context(inputFormatCtx);
         qDebug("close() free inputFormatCtx");
    }
    inputFormatCtx = NULL;
    return true;
}

bool MyFfPlayer::pause(){
    playing = false;
    if(inputFormatCtx && canPause()){
       av_read_pause(inputFormatCtx);
       flag_pause = 1;

    }
    return opened;
}
bool MyFfPlayer::isOpened(){
    return opened;
}
bool MyFfPlayer::isPlaying(){
    return playing;
}

bool MyFfPlayer::canPause(){
    return (inputFormatCtx->iformat->read_pause==NULL);
}

bool MyFfPlayer::canSeek(){
     //return (inputFormatCtx->iformat->read_seek==NULL);
     return (inputFormatCtx->iformat->read_seek >= 0);
}

int64_t MyFfPlayer::position(){
    pos = pts;
    return pos;
}

bool MyFfPlayer::setPosition(int64_t position){
    if(canSeek()){
        printf("Calling setPosition:%lld\n",position);
        pos = position;

        int64_t timestamp = (float)position / 1000 * duration ;
                //* AV_TIME_BASE;
        int ret = av_seek_frame(inputFormatCtx,
                                videoIndex,timestamp,AVSEEK_FLAG_BACKWARD);
        if(ret>=0){
            return true;
        }
    }
    return false;
}

bool MyFfPlayer::setFullscreen(bool flag){
    return false;
}

