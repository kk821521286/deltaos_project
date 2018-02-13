//#include "myvlcplayer.h"
//#include <QGraphicsView>
//void vlc_log_callback(void* data, int level,const libvlc_log_t * ctx, const char* fmt, va_list args){
//    char tmp[4096];
//    vsprintf(tmp,fmt,args);		;
//    qWarning(tmp);
//}

//MyVlcPlayer::MyVlcPlayer()
//{
//    vlcInstance = libvlc_new(0,NULL);
//    vlcMediaPlayer = NULL;
//    libvlc_log_set(vlcInstance,vlc_log_callback,NULL);
//    opened = false;
//}

//MyVlcPlayer::~MyVlcPlayer(){
//    libvlc_release(vlcInstance);
//}

//bool MyVlcPlayer::open(std::string url,WId wId,int cache /* = 5000 */){
//    this->url = url;
//    this->wId = wId;
//    libvlc_media_t* m = NULL;
//    m = libvlc_media_new_location(vlcInstance,url.c_str());
//    char szOpt[256];
//    sprintf(szOpt,":network-caching=%d",cache);
//    libvlc_media_add_option(m,szOpt);
//    qWarning("Opening url:%s\n",url.c_str());
//    vlcMediaPlayer = libvlc_media_player_new_from_media(m);
//    libvlc_media_parse(m);
//    qWarning("Duration:%lld\n",libvlc_media_get_duration(m));
//    libvlc_media_release(m);

//    if(vlcMediaPlayer == NULL){
//        qWarning("Failed to create vlc media player\n");
//        return false;
//    }
//    opened = true;
//    qWarning("Duration:%lld\n",libvlc_media_player_get_length(vlcMediaPlayer));
//    libvlc_media_player_set_hwnd(vlcMediaPlayer,(void*)this->wId);
//    libvlc_media_player_play(vlcMediaPlayer);
//    return true;
//}

//bool MyVlcPlayer::play(){
//    if(vlcMediaPlayer){
//        libvlc_media_player_play(vlcMediaPlayer);
//        return true;
//    }
//    return false;
//}

//long MyVlcPlayer::length(){
//    long ret = -1;
//    if(vlcMediaPlayer){
//        ret = libvlc_media_player_get_length(vlcMediaPlayer);
//        return ret;
//    }
//    return ret;
//}
//long MyVlcPlayer::time(){
//    long ret = -1;
//    if(vlcMediaPlayer){
//        ret = libvlc_media_player_get_time(vlcMediaPlayer);
//        return ret;
//    }
//    return ret;
//}

//bool MyVlcPlayer::stop(){
//    if(vlcMediaPlayer){
//        libvlc_media_player_stop(vlcMediaPlayer);
//        return true;
//    }
//    return false;
//}

//bool MyVlcPlayer::close(){
//    opened = false;
//    if(vlcMediaPlayer){
//        libvlc_media_player_release(vlcMediaPlayer);
//        vlcMediaPlayer = NULL;
//        return true;
//    }
//    return true;
//}

//bool MyVlcPlayer::pause(){
//    if(vlcMediaPlayer&&libvlc_media_player_can_pause(vlcMediaPlayer)){
//        libvlc_media_player_pause(vlcMediaPlayer);
//        return true;
//    }
//    return false;
//}
//bool MyVlcPlayer::isOpened(){
//    return opened;
//}
//bool MyVlcPlayer::isPlaying(){
//    if(vlcMediaPlayer){
//        return libvlc_media_player_is_playing(vlcMediaPlayer);
//    }
//    return false;
//}

//bool MyVlcPlayer::canPause(){
//    if(vlcMediaPlayer){
//        return libvlc_media_player_can_pause(vlcMediaPlayer);
//    }
//    return false;
//}

//bool MyVlcPlayer::canSeek(){
//    if(vlcMediaPlayer){
//        return libvlc_media_player_is_seekable(vlcMediaPlayer);
//    }
//    return false;
//}

//float MyVlcPlayer::position(){
//    if(vlcMediaPlayer){
//        return libvlc_media_player_get_position(vlcMediaPlayer);
//    }
//    return 0.0;
//}

//bool MyVlcPlayer::setPosition(float position){
//    if(vlcMediaPlayer){
//        libvlc_media_player_set_position(vlcMediaPlayer,position);
//        return true;
//    }
//    return false;
//}

//bool MyVlcPlayer::setFullscreen(bool flag){
//    return false;
//}
