#ifndef MCRCLIENTIMPL_H
#define MCRCLIENTIMPL_H

#include"mcrclient.h"


//MCRClientImpl class declare
class  MCRClientImpl : public MCRClient
{

public:
   MCRClientImpl();
   //function 1
   virtual void getModuleCount(QVector<int> &array,const char* url,ErrorInfo &e);
    //function 2
   virtual  void setModuleCount(int Count,const char* url,ErrorInfo &e);

    //function 3
   virtual void getModuleConfig(ModuleConfig &mConf,int Mid,const char* url,ErrorInfo &e);
    //function 4
   virtual void setModuleConfig(ModuleConfig * Config,const char* url,ErrorInfo &e);

    //function 5
  virtual void getChannelConfig(ChannelConfig &cConf,int Mid,int Cid,const char* url,ErrorInfo &e);
    //fucntion 6
  virtual  void setChannelConfig(ChannelConfig *Config,const char* url,ErrorInfo &e);

    //function 7
    virtual void getChannelFileCount(int &fCount,int Mid,int Cid,const char* url,ErrorInfo &e);
    //function 8
   virtual void getChannelFileList(int Mid,int Cid,vector<FileInfo> &list,const char* url,ErrorInfo &e);

    //function 9
  virtual  void getChannelStreamStatus(int &Status,int Mid,int Cid,const char* url,ErrorInfo &e);
    //function 10
   virtual void getChannelRecordStatus(int &Status,int Mid,int Cid,const char* url,ErrorInfo &e);
    //function 11
  virtual void ctrlChannelRecord(int Mid,int Cid,int Cmd,const char* url,ErrorInfo &e);

    //function 12
  virtual  void getChannelStreamConfig(StreamConfig &sConf,int Mid,int Cid,const char* url,ErrorInfo &e);
    //function 13
   virtual void setChannelStreamConfig(StreamConfig *Config,const char* url,ErrorInfo &e);

    //function 14
  virtual void getChannelRecordConfig(RecordConfig &rConf, int Mid,int Cid,const char* url,ErrorInfo &e);
    //function 15
   virtual void setChannelRecordConfig(RecordConfig *Config,const char* url,ErrorInfo &e);

    //function 16
  virtual void deleteChannelFile(int Mid,int Cid,std::string FileUrl,const char* url,ErrorInfo &e);
    //function 17
   virtual void deleteChannelFileAll(int Mid,int Cid,const char* url,ErrorInfo &e);
   //function 18
   virtual void deleteChannel(int Mid,int Cid,const char* url,ErrorInfo &e);
   //function 19
   virtual void deleteModule(int Mid,const char* url,ErrorInfo &e);
   //function 20
   virtual void addModule(ModuleConfig *mConf,const char* url,ErrorInfo &e) ;
   //function 21
   virtual void addChannel(ChannelConfig *cConf,int &GetCid,const char* url,ErrorInfo &e) ;
   //function 22
   virtual void getChannelCount(int Mid,QVector<int> &array,const char* url,ErrorInfo &e);
   //function 23
   virtual void addChannelRecord(RecordConfig *rConf,const char*url,ErrorInfo &e);
   //function 24
   virtual void deleteChannelRecord(int Mid,int Cid,const char* url,ErrorInfo &e);
   //function 25
   virtual void reboot(const char*url,ErrorInfo &e);
   //function 26
    virtual void update(QString *pStr,const char* url,ErrorInfo &e);
    //function 27
     virtual void checkUpdateStatus(int &Status,int Mid,int Cid,const char* url,ErrorInfo &e) ;

   //function 28
   virtual void closePlayer(const char*url,ErrorInfo &e) ;
   //function 29
   virtual void openPlayer(QString *streamUrl,const char *url,int isLive,ErrorInfo &e);
   //function 30
   virtual void getStatusIsOpenedPlayer(int &isOpened,const char *url,ErrorInfo &e) ;
   //function 31
   virtual void getStatusIsPlayingPlayer(int &isPlaying,const char *url,ErrorInfo &e) ;
   //function 32
   virtual void getStatusCanSeekPlayer(int &canSeek, const char *url , ErrorInfo &e) ;
   //function 33
   virtual void getFileLengthPlayer(int64_t &length, const char *url, ErrorInfo &e) ;
   //function 34
   virtual void getFileTimePlayer(int64_t &time,const char *url,ErrorInfo &e);
   //function 35
   virtual void playFilePlayer(const char *url,ErrorInfo &e);
   //function 36
   virtual void setPosition(int64_t pos,const char *url,ErrorInfo &e);
};

#endif // MCRCLIENTIMPL_H
