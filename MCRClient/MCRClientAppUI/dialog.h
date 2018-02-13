#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QFrame>
#include <QStandardItemModel>

#include <sys/mman.h>
#include<tickLib.h>
#include<semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "moduleitemdelegate.h"
#include "fileitemdelegate.h"
#include "mcrclientqt.h"
#include "myffplayer.h"

#include <string>

#include "download.h"
#include <map>
#include <vector>
#include <QProcess>
#include <QSettings>
#include <QToolButton>
#include <Qthread>
#include <QDebug>
#include "channeldialog.h"
#include"debug.h"

#define MODULENUM 4
#define CHANNELNUM 12
namespace Ui {
class Dialog;
}

enum ResizeRegion
{
    Default,
    North,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest
};
typedef struct {
    int mid;
    int cid;
    QString strID;
}channelID;
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void setButtonHide();
    QSettings* config;
    QSettings* mCount;
    QSettings* mConfig;
    int btnConfig;
    QStandardItemModel* modelTreeModule;
    QStandardItemModel* modelTableFile;
    ModuleItemDelegate* moduleItemDelegate;
    FileItemDelegate* fileItemDelegate;
    MCRClient* mcrClient;
    MyFfPlayer *mPlayer;

    std::string moduleServerUrl;
    //与译码器进程通讯
    const char* ipcUrl;
    QString mid_array;
    ChannelConfig* currentChannelConfig;
    ChannelInfoData cInfoData_isDelete;
    std::map<std::string,Download*> downloadMap;
    std::map<QString,int> statusMap;
    std::vector<FileInfo> fileInfoList;
    std::vector<FileInfo> filteredFileInfoList;
    int currentPageNo;
    int pageSize;
    int moduleCount;
    int playing_mid;
    int playing_cid;
    //libffmpeg
   // MyFfPlayer* player;
    int playerTimer;
    int downloadTimer;
    int channelTimer;
    int positionTimer;
    int updateStatusTimer;
    int updateGroupStatusTimer;
    int sinaleThreadUpdateStatusTimer;
    int proIsRunning;
    int showVedioes;
    float position_value ;
    float value_position[2];
    int flagLive;//当直播时，置flagLive值为1,非直播时,置flagLive值为0

    unsigned char * pData; //共享内存指向的指针

    bool bool_play; //播放开关，用于调试


    QDialog *desktop;

    std::string httpUrl;
    std::string localUrl;
    QString url_bak;



    QDialog* alertDlg;
    QLabel* alertLabel;
    QRect showVedioRect;
    int VEDIOWIDTH;
    int VEDIOHEIGHT;
    int fVEDIOWIDTH;
    int fVEDIOHEIGHT;

    int SCREENWIDTH;
    int SCREENHEIGHT;
    std::string downloadPath;
    QImage *image;
    QImage *image_null;
    QPoint mLastMousePosition;
    bool mMoving;

    //Shm video show
    QImage* imageVideo;
    QStandardItem* module_1channelsArray[5];
    QStandardItem* module_2channelsArray[5];


    void getModules();
    void deleteMid(int delMid);
   // void getChannels(QStandardItem* itemModule);
    void updateProStatus();
    void updateModules();
    void freshModules();
    void traverseTree();
    void traverseChannelsUpdateStatus(QStandardItem *itemChannel);
    void freshChannels(ChannelInfoData &channelInfoData);
    void updateChannels(QStandardItem* itemModule);
    void updatePlayerUI();
    void updateDownload();
    void updatePosition();
    void updateFileTable();
  //  void updateChannelAlias(QStandardItem* item,ChannelConfig &cConf);
    void asyncUpdateChannelStatus(ChannelConfig channelConfig);
    void asyncUpdateChannelStatusSingleThread(QStandardItem *itemChannel);

    void asyncCheckChannelStatusSingle();
    void asyncGetChannelFileList();
    void asyncUpdateProStatus(QStandardItem* itemChannel);
    void asyncDeleteChannelFileList(QModelIndexList indexList);
    void asyncDeleteChannelAllFile(QModelIndexList indexList);
    void asyncDeleteChannel(QModelIndexList indexList);
    void asyncDeleteModule(QModelIndexList indexList);
    void asyncShowVedio();
    void filterFileInfoList();




    static void checkChannelStatus(void* ctx);
    static void checkFileList(void* ctx);
    static void deleteFileList(void* ctx);
    static void deleteAllFile(void* ctx);
    //----
    static void deleteModule(void* ctx);
    static void deleteChannel(void* ctx);
    static void checkChannelStatusSingle(void* ctx);
    static void showVedio(void * ctx);

    static void checkChannelStatusInModule(void*ctx);
    void asyncCheckChannelStatusInModule(QStandardItem* moduleItem);
    //----
    //CheckStatusThread* checkStatusThread;
    //CheckFileListThread* checkFileListThread;
    QMutex* mutex;
    QMutex* statusMapMutex;
    WorkerThread* checkStatusThread;
    WorkerThread* checkFileListThread;
    WorkerThread* deleteFileListThread;
    WorkerThread* deleteAllFileThread;
    //----
    WorkerThread* deleteModuleThread;
    WorkerThread* deleteChannelThread;
    WorkerThread* threadVedio;
    WorkerThread* channelStatusThread_1[CHANNELNUM];
    WorkerThread* channelStatusThread_2[CHANNELNUM];
    WorkerThread* channelStatusThread_3[CHANNELNUM];
    WorkerThread* channelStatusThread_4[CHANNELNUM];

    WorkerThread* checkGroupStatusThread[MODULENUM];
    //----
    std::string getLocalPath(FileInfoData fid);

   // ChannelDialog* channelDlg;

    int currentPlayerType;
    void keyPressEvent(QKeyEvent *);
    bool fullScreen;
protected:
    void timerEvent(QTimerEvent * event);
    bool eventFilter(QObject *target, QEvent *event);

private slots:
    void finish_process(int exitCode, QProcess::ExitStatus exitStatus);

    void start_process();

    void on_pushButton_RecordStart_clicked();

    void on_pushButton_RecordStop_clicked();

    void on_treeView_Module_clicked(const QModelIndex &index);


    void on_pushButton_DownloadStart_clicked();

    void on_pushButton_DownloadStop_clicked();

    void on_pushButton_DeleteFile_clicked();

    void on_pushButton_PageUp_clicked();

    void on_pushButton_PageDown_clicked();

    void on_tableView_File_clicked(const QModelIndex &index);

//    void on_tableView_File_doubleClicked(const QModelIndex &index);

    void on_pushButton_PlayerStart_clicked();

    void on_pushButton_PlayerStop_clicked();

    void on_pushButton_PlayerFullscreen_clicked();

//    void on_horizontalSlider_Player_valueChanged(int value);

    void on_horizontalSlider_Player_sliderMoved(int position);

//    void on_dateEdit_Start_editingFinished();

//    void on_dateEdit_Stop_editingFinished();

    void on_dateEdit_Start_dateChanged(const QDate &date);

    void on_dateEdit_Stop_dateChanged(const QDate &date);

    void on_pushButton_DownloadDir_clicked();

    void on_pushButton_Delete_clicked();

    void on_pushButton_Add_clicked();

    void on_pushButton_DeleteAllFile_clicked();

    void slotShowSmall();

    void slotHelp();

    void slotShowMaxRestore();
    void on_pushButton_update_clicked();

    void on_pushButton_PlayerPause_clicked();



    void on_pBt_shift_clicked();

private:
    Ui::Dialog *ui;
private:
    bool isMax_;

    QPixmap maxPixmap_;
    QPixmap restorePixmap_;
};


typedef struct CheckStatusThreadParam{
    Dialog* dlg;
   // QStandardItem* item;
    ChannelConfig channelConfig;
}CheckStatusThreadParam;
typedef struct CheckGroupStatusThreadParam{
    Dialog* dlg;
    QStandardItem* ModuleItem;
}CheckGroupStatusThreadParam;
typedef struct CheckAliasThreadParam{
    Dialog* dlg;
    QStandardItem* item;
}CheckAliasThreadParam;
typedef struct CheckFileListThreadParam{
    Dialog* dlg;
    QStandardItem* item;
}CheckFileListThreadParam;
typedef struct DeleteFileListThreadParam{
    Dialog* dlg;
    QModelIndexList indexList;
}DeleteFileListThreadParam;
typedef struct DeleteAllFileThreadParam{
    Dialog* dlg;
    QModelIndexList indexList;
}DeleteAllFileThreadParam;
//----
typedef struct DeleteModuleThreadParam{
   Dialog* dlg;
   QModelIndexList indexList;
}DeleteModuleThreadParam;
typedef struct DeleteChannelThreadParam{
   Dialog* dlg;
   QModelIndexList indexList;
}DeleteChannelThreadParam;
//----
#endif // DIALOG_H
