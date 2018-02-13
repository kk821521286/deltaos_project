#ifndef MCRCLIENT_H
#define MCRCLIENT_H
#include "mcrclient_global.h"
#include <QVector>
#include <stdint.h>
#include<QLinkedList>
#include<iostream>
using namespace std;

    typedef enum {
        RecordStatusStopped = 0,
        RecordStatusRunning = 1
    }RecordStatus;


    typedef enum {
        StreamStatusStopped = 0,
        StreamStatusRunning = 1
    }StreamStatus;


    typedef enum {
        RecordCmdStop = 0,
        RecordCmdStart = 1
    }RecordCmd;






//module config struct
typedef struct ModuleConfig{
    int mid;
    std::string name;
    std::string alias;
    std::string ip;
   // QVector<int> cidArray;
}ModuleConfig;
//channel config struct
typedef struct ChannelConfig{
    int mid;
    int cid;
    std::string name;
    std::string alias;
    std::string ip;
    int type;
    std::string streamUrl;
    std::string streamServerUrl;
    std::string recordServerUrl;
    std::string fileServerUrl;
}ChannelConfig;
//file information struct
typedef struct FileInfo {
    std::string localUrl;
    std::string httpURL;
    std::string downloadUrl;
    int size;
    int cTime;
    int eTime;
    int type;
}FileInfo;
//record config struct
typedef struct RecordConfig {
    int mid;
    int cid;
    std::string srcUrl;
    std::string prefix;
    std::string type;
    std::string timeServer;
    int segment;
}RecordConfig;
//stream config struct
typedef struct StreamConfig {
   int mid;
   int cid;
   int media_codec1_video_width;
   int media_codec1_video_height;
   int media_codec1_video_bitrate;
   int media_codec1_video_maxbitrate;
   int media_capture_audio_samplerate;
   int media_codec1_audio_bitrate;
}StreamConfig;
//error struct
typedef struct ErrorInfo {
    int result;
    std::string info;
    ErrorInfo(){
        result = 0;
        info = "";
    }
}ErrorInfo;

class MCRCLIENTSHARED_EXPORT MCRClient
{
public:
    virtual ~MCRClient(){}
public:
    //function 1
    virtual void getModuleCount(QVector<int> &array,const char* url,ErrorInfo &e)=0;
     //function 2
    virtual  void setModuleCount(int Count,const char* url,ErrorInfo &e)=0;

     //function 3
    virtual void getModuleConfig(ModuleConfig &mConf,int Mid,const char* url,ErrorInfo &e)=0;
     //function 4
    virtual void setModuleConfig(ModuleConfig * Config,const char* url,ErrorInfo &e)=0;

     //function 5
   virtual void getChannelConfig(ChannelConfig &cConf,int Mid,int Cid,const char* url,ErrorInfo &e)=0;
     //fucntion 6
   virtual  void setChannelConfig(ChannelConfig *Config,const char* url,ErrorInfo &e)=0;

     //function 7
     virtual void getChannelFileCount(int &fCount,int Mid,int Cid,const char* url,ErrorInfo &e)=0;
     //function 8
    virtual void getChannelFileList(int Mid,int Cid,vector<FileInfo> &list,const char* url,ErrorInfo &e)=0;

     //function 9
   virtual  void getChannelStreamStatus(int &Status,int Mid,int Cid,const char* url,ErrorInfo &e)=0;
     //function 10
    virtual void getChannelRecordStatus(int &Status,int Mid,int Cid,const char* url,ErrorInfo &e)=0;
     //function 11
   virtual void ctrlChannelRecord(int Mid,int Cid,int Cmd,const char* url,ErrorInfo &e)=0;

     //function 12
   virtual  void getChannelStreamConfig(StreamConfig &sConf,int Mid,int Cid,const char* url,ErrorInfo &e)=0;
     //function 13
    virtual void setChannelStreamConfig(StreamConfig *Config,const char* url,ErrorInfo &e)=0;

     //function 14
   virtual void getChannelRecordConfig(RecordConfig &rConf, int Mid,int Cid,const char* url,ErrorInfo &e)=0;
     //function 15
    virtual void setChannelRecordConfig(RecordConfig *Config,const char* url,ErrorInfo &e)=0;

     //function 16
   virtual void deleteChannelFile(int Mid,int Cid,std::string FileUrl,const char* url,ErrorInfo &e)=0;
     //function 17
    virtual void deleteChannelFileAll(int Mid,int Cid,const char* url,ErrorInfo &e)=0;
    //function 18
    virtual void deleteChannel(int Mid,int Cid,const char* url,ErrorInfo &e) = 0;
    //function 19
    virtual void deleteModule(int Mid,const char* url,ErrorInfo &e) = 0;
    //function 20
    virtual void addModule(ModuleConfig *mConf,const char* url,ErrorInfo &e) = 0;
    //function 21
    virtual void addChannel(ChannelConfig *cConf,int &GetCid,const char* url,ErrorInfo &e) = 0;
    //function 22
    virtual void getChannelCount(int Mid,QVector<int> &array,const char* url,ErrorInfo &e) = 0;
    //function 23
    virtual void addChannelRecord(RecordConfig *rConf,const char*url,ErrorInfo &e) = 0;
    //function 24
    virtual void deleteChannelRecord(int Mid,int Cid,const char* url,ErrorInfo &e) = 0;
    //function 25
    virtual void reboot(const char*url,ErrorInfo &e) = 0;
    //function 26
          virtual void update(QString *pStr,const char* url,ErrorInfo &e) = 0;
     //function 27
      virtual void checkUpdateStatus(int &Status,int Mid,int Cid,const char* url,ErrorInfo &e) = 0;

    // IPC of two process
    //function 28
    virtual void closePlayer(const char*url,ErrorInfo &e) = 0;
    //function 29
    virtual void openPlayer(QString *streamUrl,const char *url,int isLive,ErrorInfo &e) = 0;
    //function 30
    virtual void getStatusIsOpenedPlayer(int &isOpened,const char *url,ErrorInfo &e) = 0;
    //function 31
    virtual void getStatusIsPlayingPlayer(int &isPlaying,const char *url,ErrorInfo &e) = 0;
    //function 32
    virtual void getStatusCanSeekPlayer(int &canSeek,const char *url ,ErrorInfo &e) = 0;
    //function 33
    virtual void getFileLengthPlayer(int64_t &length,const char *url,ErrorInfo &e) = 0;
    //function 34
    virtual void getFileTimePlayer(int64_t &time,const char *url,ErrorInfo &e) = 0;
    //function 35
    virtual void playFilePlayer(const char *url,ErrorInfo &e) = 0;
    //function 36
    virtual void setPosition(int64_t pos,const char *url,ErrorInfo &e) = 0;
};
class MCRCLIENTSHARED_EXPORT MCRClientFactory{
public:
    static MCRClient* CreateInstance();
};
#endif // MCRCLIENT_H

