#include "dialog.h"
#include "ui_dialog.h"
#include"channeldialog.h"
#include"moduledialog.h"
#include"addchanneldialog.h"
#include"info.h"
#include"updatedialog.h"
#include"addmoduledialog.h"
//#include<windows.h>
#include <QDir>
#include<QInputDialog>
#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QVariant>
#include <QScrollBar>
#include <QMessageBox>
#include <QKeyEvent>
#include <QProcess>
#include <vxWorks.h>
#include <semLib.h>
#include<rtpLib.h>
#include <taskLib.h>
#include <errnoLib.h>
#include "usrSysReboot.h"
#include <QFileInfo>
#include <QDesktopServices>
#include <shiftlogin.h>
#include <shiftmessagebox.h>
SEM_ID semIdShare;
int g_dbg_level;
FILE *g_log_fp;
//sem_t * semIdShare = 0;
#define GBYTE (1024*1024*1024)
#define MBYTE (1024*1024)
#define KBYTE (1024)
#define WIDTH 1600
#define HIGH 1200
#define MYFFPLAYER
/*
 * FileName: dialog.cpp
 * Author: kangkai
 * Version: 2.0
 * Date: 2016/2/22
 * Description:
 * Function List:
 */
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    g_dbg_level = 15;
    g_log_fp = fopen("mcrclientapp.log","a+");
    if (g_log_fp == NULL)
    {
        printf("open log file error\n");
    }

    QString aDir = QCoreApplication::applicationDirPath();
    aDir += "/config.ini";
    qWarning(aDir.toStdString().c_str());
    config = new QSettings(aDir,QSettings::IniFormat);

    moduleServerUrl = config->value("/system/server").toString().toStdString();
    if(moduleServerUrl==""){
        moduleServerUrl = "http://127.0.0.1/soapserver.php";
    }
    ipcUrl = "http://127.0.0.1:5678";
    downloadPath = config->value("/system/download_path").toString().toStdString();
    if(downloadPath==""){
        downloadPath = "download";
    }
    btnConfig = 0;
    btnConfig = config->value("system/button_config").toInt(0);

    if(btnConfig == 0){
        ui->pushButton_Add->hide();
        ui->pushButton_Delete->hide();
    }
    setButtonHide();
    std::string windowTitle = config->value("/system/title").toString().toStdString();
    if(windowTitle!=""){
        this->setWindowTitle(QObject::tr(windowTitle.c_str()));
    }


    Qt::WindowFlags flags;

    flags = windowFlags();
#ifdef CUSTOM_TITLEBAR
    flags = flags &(~Qt::WindowTitleHint);
    flags = flags &(~Qt::WindowSystemMenuHint);
    flags = flags | Qt::FramelessWindowHint;
#else
   // ui->widget_Title->hide();
#endif
    flags =  Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint  ;
   // flags = flags | Qt::WindowMinMaxButtonsHint;
    setWindowFlags( flags );    
   // setWindowModality(Qt::WindowModal);
    alertDlg = new QDialog();



        flags = alertDlg->windowFlags();
        flags = flags &(~Qt::WindowTitleHint);
        flags = flags &(~Qt::WindowSystemMenuHint);
        flags = flags | Qt::FramelessWindowHint;
        alertDlg->setWindowFlags( flags );
        alertDlg->setModal(true);

    alertLabel = new QLabel();
    alertLabel->setAlignment(Qt::AlignCenter);
    QHBoxLayout* alertLayout = new QHBoxLayout;
  alertLayout->addWidget(alertLabel);
    alertDlg->setLayout(alertLayout);

    alertLabel->setStyleSheet(" \
                      QLabel{\
                      background-color:white; \
                      border-width: 1px;\
                      border-style: solid;\
                      border-radius:10px;\
                      border-color: rgb(255, 170, 0);\
                      }"
                              );

    pageSize = 10;
    currentChannelConfig = NULL;


    //define max button min button close button
        isMax_ = false;
        QPixmap helpPixmap = this->style()->standardPixmap(QStyle::SP_TitleBarContextHelpButton);
        ui->pushButton_help->setIcon(helpPixmap);
        connect(ui->pushButton_help,SIGNAL(clicked()),this,SLOT(slotHelp()));


        QPixmap minPixmap = this->style()->standardPixmap(QStyle::SP_TitleBarMinButton);
        ui->pushButton_min->setIcon(minPixmap);
        connect(ui->pushButton_min, SIGNAL(clicked()), this, SLOT(slotShowSmall()));

        maxPixmap_ = this->style()->standardPixmap(QStyle::SP_TitleBarMaxButton);
        restorePixmap_ = this->style()->standardPixmap(QStyle::SP_TitleBarNormalButton);
        ui->pushButton_max->setIcon(maxPixmap_);
        connect(ui->pushButton_max, SIGNAL(clicked()), this, SLOT(slotShowMaxRestore()));


        QPixmap closePixmap = this->style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
        ui->pushButton_close->setIcon(closePixmap);
        connect(ui->pushButton_close, SIGNAL(clicked()), this, SLOT(close()));

        QPixmap desktopPixmap = this->style()->standardPixmap(QStyle::SP_DesktopIcon);
        ui->label_logo->setPixmap(QPixmap(desktopPixmap));
        ui->label_logo->setFixedSize(20,20);
        ui->label_logo->setScaledContents(false);
        ui->widget_Title->setContentsMargins(0,0,0,10);
        ui->widget_Title->setAutoFillBackground(false);


    //set the qss
    QFile styleSheet(":/qss/default.qss");
    if (!styleSheet.open(QIODevice::ReadOnly))
    {
        qWarning("Can't open the style sheet file.");
    }else{
        this->setStyleSheet(styleSheet.readAll());
        styleSheet.close();
    }
   // QMutex* mutex = new QMutex;
    checkStatusThread = NULL;

    ui->treeView_Module->setAlternatingRowColors(true);
    modelTreeModule = new QStandardItemModel(1,3,ui->treeView_Module);
    ui->treeView_Module->setModel(modelTreeModule);
    modelTreeModule->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("状态")));
    modelTreeModule->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("信息")));
    modelTreeModule->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("设置")));
    ui->treeView_Module->header()->setResizeMode(1,QHeaderView::Stretch);
    ui->treeView_Module->setIndentation(5);
    ui->treeView_Module->setColumnWidth(0,30);
    ui->treeView_Module->setColumnWidth(2,100);
    ui->treeView_Module->setHeaderHidden(true);
    ui->treeView_Module->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView_Module->setIconSize(QSize(40,80));


    modelTableFile = new QStandardItemModel(pageSize,5,ui->tableView_File);
    int i,j;
    for(i=0;i<pageSize;i++){
        for(j=0;j<5;j++){
            QStandardItem* item = new QStandardItem;
            modelTableFile->setItem(i,j,item);
        }
    }
    ui->tableView_File->setModel(modelTableFile);
    modelTableFile->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("序号")));
    modelTableFile->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("文件名")));
    modelTableFile->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("文件时长")));
    modelTableFile->setHorizontalHeaderItem(3,new QStandardItem(QObject::tr("文件大小")));
    modelTableFile->setHorizontalHeaderItem(4,new QStandardItem(QObject::tr("记录时间")));
//    modelTableFile->setHorizontalHeaderItem(5,new QStandardItem(QObject::tr("下载状态")));

    ui->tableView_File->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
    ui->tableView_File->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_File->verticalHeader()->hide();

    for(i = 0;i<10;i++){
        ui->tableView_File->setRowHeight(i,20);
    }
    ui->tableView_File->setColumnWidth(0,30);
    ui->tableView_File->setColumnWidth(4,150);


    mcrClient = MCRClientFactory::CreateInstance();
#ifdef MYFFPLAYER
    mPlayer = new MyFfPlayer();
#endif



    statusMapMutex = new QMutex;

    for(i = 0;i<CHANNELNUM;i++){
        channelStatusThread_1[i] = NULL;
        channelStatusThread_2[i] = NULL;
        channelStatusThread_3[i] = NULL;
        channelStatusThread_4[i] = NULL;
    }
    for(i = 0;i<MODULENUM;i++){
        checkGroupStatusThread[i] = NULL;
    }

    freshModules();


#ifdef CUSTOM_TREE_DELEGATE
    moduleItemDelegate = new ModuleItemDelegate(ui->treeView_Module);
    ui->treeView_Module->setItemDelegate(moduleItemDelegate);
#endif
    fileItemDelegate = new FileItemDelegate;
    ui->tableView_File->setItemDelegate(fileItemDelegate);
   // player = new MyFfPlayer();
    playerTimer = 0;
    proIsRunning = 0;
    showVedioes = 0;

    fullScreen = false;
    //flag for delete all file
    playing_mid = -1;
    playing_cid = -1;

    ui->horizontalSlider_Player->setRange(0,1000);
    downloadTimer = startTimer(2000);

    proIsRunning = startTimer(5000);
    updateStatusTimer = startTimer(5000);
    sinaleThreadUpdateStatusTimer = startTimer(10000);
    updateGroupStatusTimer = startTimer(5000);

    //channelDlg = new ChannelDialog(this);
    currentPlayerType = 0;
    ui->dateEdit_Start->setDate(QDate::fromString("2000-01-01"));
    ui->dateEdit_Stop->setDate(QDate::currentDate());
    ui->pushButton_DownloadStart->setText(QObject::tr("开始下载"));
    ui->pushButton_DownloadStop->setText(QObject::tr("停止下载"));
    ui->pushButton_DownloadDir->setText(QObject::tr("下载目录"));
    ui->pushButton_DownloadDir->hide();
    ui->label_StartTime->setText(QObject::tr("起始时间:"));
    ui->label_StopTime->setText(QObject::tr("结束时间:"));
    ui->pushButton_PlayerStart->setText(QObject::tr("播放"));
    ui->pushButton_PlayerPause->setText(QObject::tr("暂停"));
    ui->pushButton_PlayerPause->hide();
    ui->pushButton_PlayerStop->setText(QObject::tr("停止"));
    ui->pushButton_PlayerFullscreen->setText(QObject::tr("全屏"));
    ui->pushButton_DeleteFile->setText(QObject::tr("删除文件"));
    ui->pushButton_DeleteAllFile->setText(QObject::tr("全部删除"));
    ui->pushButton_DeleteAllFile->hide();
    ui->pushButton_Add->setText(QObject::tr("增加模块/通道"));
    //ui->pushButton_Add->hide();
    ui->pushButton_Delete->setText(QObject::tr("删除模块/通道"));
    //ui->pushButton_Delete->hide();
    ui->pushButton_PageUp->setText(QObject::tr("上一页"));
    ui->pushButton_PageDown->setText(QObject::tr("下一页"));
    ui->pushButton_RecordStart->setText(QObject::tr("开始录制"));
    ui->pushButton_RecordStop->setText(QObject::tr("停止录制"));
    ui->pushButton_update->setText(QObject::tr("升级系统"));


//    checkStatusThread = new WorkerThread(this,Dialog::checkChannelStatusSingle);
//    checkStatusThread->start();
//     image = new QImage(":/image/loading.png");
//     image_null = new QImage();
     ui->horizontalSlider_Player->installEventFilter(this);


     setMouseTracking(false);

//     int nRtp;
//     // 注意这里的文件路径使用虚拟路径，否则将出现nRtp=-1不成功的状态！
//     const char *argv[] = {"/ahci00:1/ffplayer.vxe", NULL};
//     const char *envp[] = {"HEAP_INITIAL_SIZE=0x20000", "HEAP_MAX_SIZE=0x100000", NULL};
//     nRtp = (int)rtpSpawn(argv[0], argv, envp, 100, 0x10000, 0, 0);
//     if (nRtp == -1)
//     {
//      // printErrno(errnoGet());
//       printf ("The RTP task not spawned!\n");
//     }

    semIdShare = semOpen("/mysem",SEM_TYPE_MUTEX,0,0,OM_CREATE|OM_EXCL,(void*)"sem_msg");
    if(semIdShare == NULL){
        qWarning("create sem failed in dialog");
    }
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

     /* close the file descriptor; the mapping is not impacted by this */
    imageVideo = new QImage(WIDTH,HIGH,QImage::Format_RGB32);
     //close(fd);

     bool_play = false;

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setButtonHide()
{
    ui->pushButton_update->hide();
    ui->pushButton_Add->hide();
    ui->pushButton_Delete->hide();

    ui->pushButton_DownloadStart->hide();
    ui->pushButton_DownloadStop->hide();
}
/*
 * Function: updateModules()
 * Description: the function is used for fresh the modules of the server you connected.
 * every time you open the software ,the function will be called.
 * Calls:
 *  1.getModuleCount();
 *  2.getModuleConfig();
 *  3.updateChannels();
 * Input: none
 * Output:none
 * Return:none
 * Others:none
 */
/*
void Dialog::updateModules(){
//    ErrorInfo e;
//    QVector<int> midArray;
//    //int nModule = 0;


//    mcrClient->getModuleCount(midArray,moduleServerUrl.c_str(),e);
//    if(e.result!=0){
//        qWarning("getModuleCount Failed!\n");
//        return;
//    }
//    int i;
//    moduleCount = midArray.size();
//    qDebug("boot module count is %d",midArray.size());
//    for(i = 0; i<midArray.size(); i++){
//        QStandardItem* itemModule = new QStandardItem;
//        QStandardItem *item = new QStandardItem;
//        QStandardItem *item1 = new QStandardItem;

//        ModuleConfig mc;// = new ModuleConfig;

//        mcrClient->getModuleConfig(mc,midArray[i],moduleServerUrl.c_str(),e);


//        ModuleInfoData moduleInfoData;
//        moduleInfoData.moduleConfig = mc;
//        itemModule->setData(QVariant::fromValue(moduleInfoData));

//        item->setData(QVariant::fromValue(moduleInfoData));
//        item1->setData(QVariant::fromValue(moduleInfoData));
//#ifndef CUSTOM_TREE_DELEGATE
//        itemModule->setText(tr(mc.name.c_str()));

//       // item->setIcon(QIcon(":/image/config.png"));
//        item->setText(QObject::tr("设置模块"));
//#endif
//        updateChannels(itemModule);
//        modelTreeModule->setItem(i,0,itemModule);
//        modelTreeModule->setItem(i,1,item1);
//        modelTreeModule->setItem(i,2,item);


//    }
}
*/
void Dialog::getModules()
{
    ErrorInfo e;
    QVector<int> midArray;
   // int nModule = 0;


    mcrClient->getModuleCount(midArray,moduleServerUrl.c_str(),e);
    if(e.result!=0){
        qWarning("getModuleCount Failed!\n");
        return;
    }
    int i;
    moduleCount = midArray.size();
    qDebug("boot module count is %d",midArray.size());
    for(i = 0; i<midArray.size(); i++){
        QStandardItem* itemModule = new QStandardItem;


        ModuleConfig mc;// = new ModuleConfig;

        mcrClient->getModuleConfig(mc,midArray[i],moduleServerUrl.c_str(),e);


        ModuleInfoData moduleInfoData;
        moduleInfoData.moduleConfig = mc;
        itemModule->setData(QVariant::fromValue(moduleInfoData));
       // getChannels(itemModule);
    }
}
/*
 * Function: freshModules()
 * Description: the function is called when you Add(Delete) a Module(Channel).it will fresh
 * the Modules.
 * Calls:
 *  1.getModuleCount();
 *  2.getModuleConfig();
 *  3.updateChannels();
 * Input: none
 * Output:none
 * Return:none
 * Others:none
 */
void Dialog::freshModules()
{
    ErrorInfo e;
    //int nModule = 0;
   // QVector<int> midArray;

    modelTreeModule = new QStandardItemModel(1,3,ui->treeView_Module);
    ui->treeView_Module->setModel(modelTreeModule);
    modelTreeModule->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("状态")));
    modelTreeModule->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("信息")));
    modelTreeModule->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("设置")));
    ui->treeView_Module->header()->setResizeMode(1,QHeaderView::Stretch);
    ui->treeView_Module->setColumnWidth(0,100);
    ui->treeView_Module->setColumnWidth(2,80);
    ui->treeView_Module->setHeaderHidden(true);
    ui->treeView_Module->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView_Module->setIconSize(QSize(40,80));

/*
   mcrClient->getModuleCount(midArray,moduleServerUrl.c_str(),e);
    if(e.result!=0){
        qWarning("getModuleCount Failed!\n");
        return;
    }
    qDebug("fresh module count is %d",midArray.size());
*/

    QString mDir = QCoreApplication::applicationDirPath();
    mDir += "/module_count.ini";

    mCount = new QSettings(mDir,QSettings::IniFormat);
    mid_array = mCount->value("/count/count").toString();

    if(mid_array.isEmpty()){

        //moduleServerUrl = "http://127.0.0.1/soapserver.php";
    }else{
        int i;

        QVector<int> tmp_mid;
        QString tmp = "";


        QByteArray ba = mid_array.toLatin1();
        const char *t = ba.data();
        char pmid[10] = {0};
        strcpy(pmid,t);
        for(i=0;i<mid_array.length();i++){

            if(pmid[i] != ','){
                tmp += pmid[i];
                if(mid_array.length() - 1 == i){

                    tmp_mid.push_back(tmp.toInt());
                }
            }else{

                tmp_mid.push_back(tmp.toInt());
                tmp = "";
            }
        }


        QVector<int>::iterator it;
        int j = 0;

        for(it=tmp_mid.begin(); it != tmp_mid.end(); it++){

            QStandardItem* itemModule = new QStandardItem;
            QStandardItem *item = new QStandardItem;
            QStandardItem *item1 = new QStandardItem;

            QString mDir = QCoreApplication::applicationDirPath();

            mDir += "/module_"+QString::number(*it)+".ini";
             qWarning("mDir === %s",mDir.toStdString().c_str());
            ModuleConfig mc;
            mConfig = new QSettings(mDir,QSettings::IniFormat);
            mc.mid = mConfig->value("/description/mid").toInt();
            mc.alias = mConfig->value("/description/alias").toString().toStdString();
            mc.name = mConfig->value("/description/name").toString().toStdString();
            mc.ip = mConfig->value("/network/ip").toString().toStdString();
//            qDebug("0 read module ini mid :%d alias :%s name:%s ip :%s",mc.mid,mc.alias.c_str(),
//                   mc.name.c_str(),mc.ip.c_str());


            ModuleInfoData moduleInfoData;
            moduleInfoData.moduleConfig = mc;
//            qDebug("1 read module ini mid :%d alias :%s name:%s ip :%s",moduleInfoData.moduleConfig.mid,moduleInfoData.moduleConfig.alias.c_str(),
//                   moduleInfoData.moduleConfig.name.c_str(),moduleInfoData.moduleConfig.ip.c_str());
            itemModule->setData(QVariant::fromValue(moduleInfoData));
            item->setData(QVariant::fromValue(moduleInfoData));
            item1->setData(QVariant::fromValue(moduleInfoData));
    #ifndef CUSTOM_TREE_DELEGATE
           // itemModule->setText(tr(mc.name.c_str()));
            item1->setText(tr(mc.name.c_str()));
            item1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            //item->setIcon(QIcon(":/image/config.png"));
            item->setText(QObject::tr("设置模块"));
    #endif
            updateChannels(itemModule);
            modelTreeModule->setItem(j,0,itemModule);
            modelTreeModule->setItem(j,1,item1);
            modelTreeModule->setItem(j,2,item);
            {
                //frash channel status of a Module
                CheckGroupStatusThreadParam* param = new CheckGroupStatusThreadParam;

                param->ModuleItem = itemModule;
                param->dlg = this;
                checkGroupStatusThread[j] = new WorkerThread(param,Dialog::checkChannelStatusInModule);
                checkGroupStatusThread[j]->setStackSize(1*1024*1024);
                checkGroupStatusThread[j]->start();
            }
            j++;
        }
    }




}
/*
 * Function: updateChannels()
 * Description: the function is to fresh the channels of certain Module.
 * Calls:
 *  1.getChannelConfig();
 * Input: itemModule
 * Output:none
 * Return:none
 * Others:none
 */
void Dialog::updateChannels(QStandardItem* itemModule){

    ErrorInfo e;
    QVariant vm = itemModule->data();
    ModuleInfoData moduleInfoData = vm.value<ModuleInfoData>();
    int i;
    //--------
   //1.获取模块的通道数
   //2.获取该模块的所有通道配置
    QVector<int> array;
    std::string serverUrl = "http://"+ moduleInfoData.moduleConfig.ip+"/soapserver.php";
    qWarning("serverUrl is %s",serverUrl.c_str());
    array.clear();
    qDebug("config.ini get mid-- %d ip-- %s",moduleInfoData.moduleConfig.mid,moduleInfoData.moduleConfig.ip.c_str());
    mcrClient->getChannelCount(moduleInfoData.moduleConfig.mid,array,serverUrl.c_str(),e);


    if(array.isEmpty()){
        return;
    }

    for(i = 0; i < array.size(); i++){

        ChannelConfig cc;// = new ModuleConfig;
        mcrClient->getChannelConfig(cc,moduleInfoData.moduleConfig.mid,array[i],serverUrl.c_str(),e);

        qDebug("getChannelConfig() mid-- %d cid-- %d",cc.mid,cc.cid);
        cc.mid = moduleInfoData.moduleConfig.mid;
        int streamStatus = 0,recordStatus = 0;
        ChannelInfoData channelInfoData;
        channelInfoData.channelConfig = cc;
        channelInfoData.streamStatus = streamStatus;
        channelInfoData.recordStatus = recordStatus;
        channelInfoData.checkStatusThread = NULL;
        channelInfoData.status = 0;
        channelInfoData.isDelete = 0;
        channelInfoData.flag = 0;
        channelInfoData.pro = NULL;


        qDebug("updateChannels mid-- %d cid-- %d",channelInfoData.channelConfig.mid,channelInfoData.channelConfig.cid);
        channelID chID;
        chID.mid = channelInfoData.channelConfig.mid;
        chID.cid = channelInfoData.channelConfig.cid;
      //  chID.strID = QString("%1_%2").arg(chID.mid,chID.cid);
        chID.strID = QString("%1").arg(chID.mid)+"_"+QString::number(chID.cid);

        statusMap.insert(std::pair<QString,int>(chID.strID,0));
        QStandardItem* itemChannel = new QStandardItem;
        CheckStatusThreadParam* param = new CheckStatusThreadParam;
       // param->item = itemChannel;
        param->channelConfig = channelInfoData.channelConfig;
        param->dlg = this;
//        channelInfoData.checkStatusThread = new WorkerThread(param,Dialog::checkChannelStatus);
//        channelInfoData.checkStatusThread->setStackSize(1*1024*1024);
//        channelInfoData.checkStatusThread->start();
/*
        if(moduleInfoData.moduleConfig.mid == 0){
           channelStatusThread_1[i] = new WorkerThread(param,Dialog::checkChannelStatus);
           channelStatusThread_1[i]->setStackSize(1024*64);
           channelStatusThread_1[i]->start();
        }else if(moduleInfoData.moduleConfig.mid == 1){
            channelStatusThread_2[i] = new WorkerThread(param,Dialog::checkChannelStatus);
            channelStatusThread_2[i]->setStackSize(1024*64);
            channelStatusThread_2[i]->start();
        }else if(moduleInfoData.moduleConfig.mid == 2){
            channelStatusThread_3[i] = new WorkerThread(param,Dialog::checkChannelStatus);
            channelStatusThread_3[i]->setStackSize(1024*64);
            channelStatusThread_3[i]->start();
        }else if(moduleInfoData.moduleConfig.mid == 3){
            channelStatusThread_4[i] = new WorkerThread(param,Dialog::checkChannelStatus);
            channelStatusThread_4[i]->setStackSize(1024*64);
            channelStatusThread_4[i]->start();
        }
*/

#ifndef CUSTOM_TREE_DELEGATE

        if(channelInfoData.streamStatus == StreamStatusRunning &&
                channelInfoData.recordStatus == RecordStatusRunning){
            itemChannel->setIcon(QIcon(":/image/playing&recording.png"));
            itemChannel->setToolTip(QObject::tr("有信号源\r\n正在录制"));

        }else if(channelInfoData.streamStatus == StreamStatusRunning &&
                 channelInfoData.recordStatus == RecordStatusStopped){
          itemChannel->setIcon(QIcon(":/image/playing&norecording.png"));
            itemChannel->setToolTip(QObject::tr("有信号源\r\n未录制"));

        }else if(channelInfoData.streamStatus == StreamStatusStopped &&
                channelInfoData.recordStatus == RecordStatusStopped){
            itemChannel->setIcon(QIcon(":/image/stop&norecording.png"));
            itemChannel->setToolTip(QObject::tr("无信号源\r\n未录制"));

        }else if(channelInfoData.streamStatus == StreamStatusStopped &&
                 channelInfoData.recordStatus == RecordStatusRunning){
             itemChannel->setIcon(QIcon(":/image/stop&recording.png"));
            itemChannel->setToolTip(QObject::tr("无信号源\r\n正在录制"));

         }
        itemChannel->setData(QVariant::fromValue(channelInfoData));
        itemChannel->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        itemModule->setChild(i,0,itemChannel);
        QStandardItem* itemChannel1 = new QStandardItem;
        std::string contentStr;
        contentStr.append(cc.alias.c_str());
        contentStr.append("\r\n");
        contentStr.append(cc.streamUrl.c_str());
        itemChannel1->setText(QObject::tr(contentStr.c_str()));
        itemChannel1->setTextAlignment(Qt::AlignLeft);
        //itemChannel1->setData(QVariant::fromValue(channelInfoData));
        itemModule->setChild(i,1,itemChannel1);
        QStandardItem* itemChannel2 = new QStandardItem;
       // itemChannel2->setIcon(QIcon(":/image/config.png"));
        itemChannel2->setText(QObject::tr("设置通道"));
        itemChannel2->setTextAlignment(Qt::AlignVCenter);
        //itemChannel2->setData(QVariant::fromValue(channelInfoData));
        itemModule->setChild(i,2,itemChannel2);

#endif

    }


    array.clear();
}
//update channel process status
//void Dialog::getChannels(QStandardItem* itemModule)
//{

//    ErrorInfo e;
//    QStandardItem *item = NULL;
//    QVariant vm = itemModule->data();
//    ModuleInfoData moduleInfoData = vm.value<ModuleInfoData>();
//    int i;
//    ChannelConfig cc;
//    for(i = 0; i < moduleInfoData.moduleConfig.cidArray.size(); i++){

//        // = new ModuleConfig;
//        mcrClient->getChannelConfig(cc,moduleInfoData.moduleConfig.mid,moduleInfoData.moduleConfig.cidArray[i],moduleServerUrl.c_str(),e);
//        //get mid cid and update channel process status
//        //NotRunning or Runnnig
//        QModelIndex index0 = modelTreeModule->index(cc.mid,0,QModelIndex());
//        QModelIndex index = modelTreeModule->index(cc.cid,0,index0);
//        QVariant v = index.data(Qt::UserRole+1);

//        if(v.canConvert<ChannelInfoData>()){
//            qDebug("get all channels");
//            ErrorInfo e;
//            ChannelInfoData cInfoData = v.value<ChannelInfoData>();
//        qDebug("cInfoData.isDelete is %d",cInfoData.isDelete);

//        if(cInfoData.pro != NULL) {
//            if(QProcess::NormalExit == cInfoData.pro->exitStatus()){
//                qDebug("pro has closed");
//                cInfoData.isDelete = 0;
//                cInfoData.pro = NULL;
//                delete  cInfoData.pro ;



//                item = modelTreeModule->itemFromIndex(index);
//                item->setData(QVariant::fromValue(cInfoData));
//            }
//        }

//       }

//    }

//}
/*
 * Function: on_pushButton_RecordStart_clicked()
 * Description: this is a slot function.when you perss StartRecord Button ,the function will be
 * called.the function will send a command to server,inform start record.
 * Calls:
 *  1.ctrlChannelRecord();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_RecordStart_clicked()
{

    QModelIndexList indexList = ui->treeView_Module->selectionModel()->selectedIndexes();
    QModelIndex index;
    int flag_delete = 0  ;
    //判断是否正在删除通道文件
    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<ChannelInfoData>()){
            ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
            flag_delete = channelInfoData.isDelete;
        }
    }

    if(flag_delete == 1){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道正在删除文件,不能开启录制！"),QMessageBox::Ok);
        return;
    }

    //-------------------
    //###############下载

    std::vector<FileInfo>::iterator it;
    for(it=filteredFileInfoList.begin();it!=filteredFileInfoList.end();it++){

            FileInfoData fid ;
            fid.downloading = false;
            if(downloadMap.find(it->downloadUrl)!=downloadMap.end()){
                fid.downloading = true;
              //  qDebug("test download true\n");
            }else{
                fid.downloading = false;
             //   qDebug("test download false\n");
            }
           if(fid.downloading){
              QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道中有文件正在下载,不能开启录制！"));
              return;
           }



    }
    //-------------------

    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        QStandardItem* itemChannel = modelTreeModule->itemFromIndex(index);
        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<ChannelInfoData>()){
            ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
            if(channelInfoData.isDelete == 1){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道正在删除文件"),QMessageBox::Ok);
                continue;
            }
            if(channelInfoData.streamStatus == StreamStatusStopped){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道无信号源，不能录制文件"),QMessageBox::Ok);
                continue;
            }
            if(channelInfoData.recordStatus == RecordStatusRunning){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("录制已经开始"),QMessageBox::Ok);
                continue;
            }
            ErrorInfo e;
            mcrClient->ctrlChannelRecord(
                        channelInfoData.channelConfig.mid,
                        channelInfoData.channelConfig.cid,
                        RecordCmdStart,
                        channelInfoData.channelConfig.recordServerUrl.c_str(),
                        e);
            if(e.result == 0){
                DBG(DBG_CRITICAL,"you successfully boot record\n");
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("启动录制成功"),QMessageBox::Ok);
            }else{
                DBG(DBG_CRITICAL,"you boot record failed\n");
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("启动录制失败"),QMessageBox::Ok);
            }
            //更新通道的状态
            {
            QVariant v = itemChannel->data();
                if(v.canConvert<ChannelInfoData>()){
                    ChannelInfoData channelInfoData = v.value<ChannelInfoData>();

                            ChannelConfig cc;
                            cc = channelInfoData.channelConfig;
                            int streamStatus = 0,recordStatus = 0;
                            ErrorInfo e;
                            e.result = 0;
                            e.info = "";

                            mcrClient->getChannelStreamStatus(streamStatus,cc.mid,cc.cid,cc.streamServerUrl.c_str(),e);
                            channelInfoData.streamOnline = (e.result==-256)?0:1;
                            mcrClient->getChannelRecordStatus(recordStatus,cc.mid,cc.cid,cc.recordServerUrl.c_str(),e);
                            channelInfoData.recordOnline = (e.result==-256)?0:1;
                            //qWarning("###############offline:[%d,%d] %d:%s",cc.mid, cc.cid, e.result, e.info.c_str());
                            if(channelInfoData.streamStatus == streamStatus &&
                                    channelInfoData.recordStatus == recordStatus){
                              qWarning("status not Changed!");
                              return;
                            }
                            if(channelInfoData.streamOnline == 0) streamStatus = 0;
                            if(channelInfoData.recordOnline == 0)  recordStatus = 0;

                            channelInfoData.streamStatus = streamStatus;
                            channelInfoData.recordStatus = recordStatus;

                            asyncUpdateChannelStatusSingleThread(itemChannel);

                            if(channelInfoData.streamStatus == StreamStatusRunning &&
                                  channelInfoData.recordStatus == RecordStatusRunning){
                                itemChannel->setIcon(QIcon(":/image/playing&recording.png"));
                                itemChannel->setToolTip(QObject::tr("有信号源\r\n正在录制"));
                            }else if(channelInfoData.streamStatus == StreamStatusRunning &&
                                  channelInfoData.recordStatus == RecordStatusStopped){
                                itemChannel->setIcon(QIcon(":/image/playing&norecording.png"));
                                  itemChannel->setToolTip(QObject::tr("有信号源\r\n未录制"));
                            }else if(channelInfoData.streamStatus == StreamStatusStopped &&
                                  channelInfoData.recordStatus == RecordStatusStopped){
                                itemChannel->setIcon(QIcon(":/image/stop&norecording.png"));
                                  itemChannel->setToolTip(QObject::tr("无信号源\r\n未录制"));
                            }else if(channelInfoData.streamStatus == StreamStatusStopped &&
                                  channelInfoData.recordStatus == RecordStatusRunning){
                                  itemChannel->setIcon(QIcon(":/image/stop&recording.png"));
                                  itemChannel->setToolTip(QObject::tr("无信号源\r\n正在录制"));
                             }
                            itemChannel->setData(QVariant::fromValue(channelInfoData));
                            itemChannel->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                        }

            }

    }
    }

}
/*
 * Function: on_pushButton_RecordStop_clicked()
 * Description: this is a slot function.when you perss StopRecord Button ,the function will be
 * called.the function will send a command to server,inform stop record.
 * Calls:
 *  1.ctrlChannelRecord();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_RecordStop_clicked()
{
    QModelIndexList indexList = ui->treeView_Module->selectionModel()->selectedIndexes();
    QModelIndex index;
    qWarning("indexList:%d\n",indexList.size());
    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        QStandardItem* itemChannel = modelTreeModule->itemFromIndex(index);
        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<ChannelInfoData>()){
            ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
            if(channelInfoData.recordStatus == RecordStatusStopped){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("录制已经停止"),QMessageBox::Ok);
                continue;
            }
            ErrorInfo e;
            mcrClient->ctrlChannelRecord(
                        channelInfoData.channelConfig.mid,
                        channelInfoData.channelConfig.cid,
                        RecordCmdStop,
                        channelInfoData.channelConfig.recordServerUrl.c_str(),
                        e);
            if(e.result == 0){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("停止录制成功"),QMessageBox::Ok);
            }else{
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("停止录制失败"),QMessageBox::Ok);
            }
            //更新通道的状态
            {
            QVariant v = itemChannel->data();
                if(v.canConvert<ChannelInfoData>()){
                    ChannelInfoData channelInfoData = v.value<ChannelInfoData>();

                            ChannelConfig cc;
                            cc = channelInfoData.channelConfig;
                            int streamStatus = 0,recordStatus = 0;
                            ErrorInfo e;
                            e.result = 0;
                            e.info = "";

                            mcrClient->getChannelStreamStatus(streamStatus,cc.mid,cc.cid,cc.streamServerUrl.c_str(),e);
                            channelInfoData.streamOnline = (e.result==-256)?0:1;
                            mcrClient->getChannelRecordStatus(recordStatus,cc.mid,cc.cid,cc.recordServerUrl.c_str(),e);
                            channelInfoData.recordOnline = (e.result==-256)?0:1;
                            //qWarning("###############offline:[%d,%d] %d:%s",cc.mid, cc.cid, e.result, e.info.c_str());
                            if(channelInfoData.streamStatus == streamStatus &&
                                    channelInfoData.recordStatus == recordStatus){
                              qWarning("status not Changed!");
                              return;
                            }
                            if(channelInfoData.streamOnline == 0) streamStatus = 0;
                            if(channelInfoData.recordOnline == 0)  recordStatus = 0;

                            channelInfoData.streamStatus = streamStatus;
                            channelInfoData.recordStatus = recordStatus;


                            asyncUpdateChannelStatusSingleThread(itemChannel);

                            if(channelInfoData.streamStatus == StreamStatusRunning &&
                                  channelInfoData.recordStatus == RecordStatusRunning){
                                itemChannel->setIcon(QIcon(":/image/playing&recording.png"));
                                itemChannel->setToolTip(QObject::tr("有信号源\r\n正在录制"));
                            }else if(channelInfoData.streamStatus == StreamStatusRunning &&
                                  channelInfoData.recordStatus == RecordStatusStopped){
                                itemChannel->setIcon(QIcon(":/image/playing&norecording.png"));
                                  itemChannel->setToolTip(QObject::tr("有信号源\r\n未录制"));
                            }else if(channelInfoData.streamStatus == StreamStatusStopped &&
                                  channelInfoData.recordStatus == RecordStatusStopped){
                                itemChannel->setIcon(QIcon(":/image/stop&norecording.png"));
                                  itemChannel->setToolTip(QObject::tr("无信号源\r\n未录制"));
                            }else if(channelInfoData.streamStatus == StreamStatusStopped &&
                                  channelInfoData.recordStatus == RecordStatusRunning){
                                itemChannel->setIcon(QIcon(":/image/stop&recording.png"));
                                  itemChannel->setToolTip(QObject::tr("无信号源\r\n正在录制"));
                             }
                            itemChannel->setData(QVariant::fromValue(channelInfoData));
                            itemChannel->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                        }

            }

        }

    }
}
/*
 * Function: updateFileTable()
 * Description: the function is to update the file table.
 * Calls:
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::updateFileTable(){
    int beginIdx = pageSize* (currentPageNo-1);
    int endIdx = -1;
    if(!filteredFileInfoList.empty()){
        endIdx = ((pageSize*(currentPageNo)) < (filteredFileInfoList.size()-1))?
                                (pageSize*(currentPageNo)) : (filteredFileInfoList.size()-1);
    }

    //modelTableFile->removeRows(0,pageSize);

    int i = 0;
    for(i=0;i<pageSize;i++){
        FileInfoData fileInfoData;
        char tmp[32];

        if(i+beginIdx<=endIdx){
            fileInfoData.fi = (FileInfo)filteredFileInfoList[i+beginIdx];
            fileInfoData.mid = currentChannelConfig->mid;
            fileInfoData.cid = currentChannelConfig->cid;



            QStandardItem* item0 = modelTableFile->item(i,0);
            sprintf(tmp,"%d",i+beginIdx+1);
            item0->setText(tmp);
            item0->setData(QVariant::fromValue(fileInfoData));

            QStandardItem* item1 = modelTableFile->item(i,1);;
            item1->setData(QVariant::fromValue(fileInfoData));
            int pos = fileInfoData.fi.localUrl.find_last_of('/')+1;
            item1->setText(fileInfoData.fi.localUrl.substr(pos).c_str());
            modelTableFile->setItem(i,1,item1);

            QStandardItem* item2 = modelTableFile->item(i,2);;
            item2->setData(QVariant::fromValue(fileInfoData));
            int pos1 = fileInfoData.fi.localUrl.find_last_of("_");



                if(fileInfoData.fi.eTime == -1){
                    std::string strDate = fileInfoData.fi.localUrl.substr(pos1+1,14);
                    QDateTime begin = QDateTime::fromString(strDate.c_str(),"yyyyMMddhhmmss");
                    // QDateTime begin = QDateTime::fromTime_t(fileInfoData.fi.cTime);
                    QDateTime end = QDateTime::fromTime_t(fileInfoData.fi.cTime);
                    int seconds = (end.toTime_t() - begin.toTime_t());
                    QTime t(seconds/3600,(seconds-seconds/3600*3600)/60,seconds%60);
                    item2->setText(t.toString("hh:mm:ss"));
                }else{
                    QDateTime begin = QDateTime::fromTime_t(fileInfoData.fi.cTime);
                    QDateTime end = QDateTime::fromTime_t(fileInfoData.fi.eTime);
                    int seconds = (end.toTime_t() - begin.toTime_t());
                    QTime t(seconds/3600,(seconds-seconds/3600*3600)/60,seconds%60);
                    item2->setText(t.toString("hh:mm:ss"));
                }



            QStandardItem* item3 = modelTableFile->item(i,3);;
            item3->setData(QVariant::fromValue(fileInfoData));
            std::string unit;
            double dsize = 1.0;
            if(fileInfoData.fi.size > GBYTE){
                dsize = fileInfoData.fi.size*dsize/GBYTE;
                unit = "GBytes";
            }else if(fileInfoData.fi.size > MBYTE){
                dsize = fileInfoData.fi.size*dsize/MBYTE;
                unit = "MBytes";
            }else if(fileInfoData.fi.size > KBYTE){
                dsize = fileInfoData.fi.size*dsize/KBYTE;
                unit = "KBytes";
            }else{
                dsize = fileInfoData.fi.size;
                unit = "Bytes";
            }
            sprintf(tmp,"%.2lf %s",dsize,unit.c_str());
            item3->setText(tmp);




            QStandardItem* item4 = modelTableFile->item(i,4);;
            item4->setData(QVariant::fromValue(fileInfoData));
            item4->setText(QDateTime::fromTime_t(fileInfoData.fi.cTime).toString("yyyy-MM-dd hh:mm:ss"));


//            QStandardItem* item5 = modelTableFile->item(i,5);;
//            item5->setData(QVariant::fromValue(fileInfoData));
//            item5->setText("");
        }else{
            int j = 0;
            for(;j<5;j++){
                modelTableFile->item(i,j)->setText("");
                modelTableFile->item(i,j)->setData(QVariant::fromValue(NULL));
            }
        }
    }


    if(filteredFileInfoList.empty()){
        ui->label_Page->setText(QObject::tr("0 / 0"));

//        for(i=0;i<pageSize;i++){
//            FileInfoData fileInfoData;
//            QStandardItem* item5 = modelTableFile->item(i,5);;
//            item5->setData(QVariant::fromValue(fileInfoData));
//            item5->setText("");
//        }
    }else{
        if(currentPageNo - 1 == ((filteredFileInfoList.size()-1)/pageSize+1)){
            char tmpStr[64];
            sprintf(tmpStr,"%d / %d",currentPageNo,(filteredFileInfoList.size()-1)/pageSize+1+1);
            ui->label_Page->setText(QObject::tr(tmpStr));
        }else{
        char tmpStr[64];
        sprintf(tmpStr,"%d / %d",currentPageNo,(filteredFileInfoList.size()-1)/pageSize+1);
        ui->label_Page->setText(QObject::tr(tmpStr));
        }
    }


}
/*
 * Function: on_treeView_Module_clicked()
 * Description: the function can  update the file table and config the channel information.
 * Calls:
 *  1.setModuleConfig();
 *  2.getModuleConfig();
 *  3.getChannelConfig();
 *  4.filterFileInfoList();
 *  5.updateFileTable();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_treeView_Module_clicked(const QModelIndex &index)
{
    qDebug()<<"on click tree0";
    ErrorInfo e;

#ifdef MYFFPLAYER
    bool isPlaying = -1 ;
    isPlaying = mPlayer->isPlaying();
    if(isPlaying){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请先停止播放"));
        return;
    }
#else
    int isPlaying = -1;
    if(!index.isValid()){
        return;
    }
    mcrClient->getStatusIsPlayingPlayer(isPlaying,ipcUrl,e);
    if(1 == isPlaying){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请先停止播放"));
        return;
    }
#endif
    if(!ui->tableView_File->isEnabled()){
        ui->tableView_File->setEnabled(true);
    }
    QStandardItem* item = modelTreeModule->itemFromIndex(index);
    if(item->parent()==NULL){
        //return;
    }
    //---------------------------------
QStandardItem *item_1 = item->parent()->child(index.row(),index.column());
    QVariant v_1 = item->data(Qt::UserRole+1);

    if(v_1.isNull()){

        qWarning(":index:%d,%d\n",index.row(),index.column());
    }
    if(v_1.canConvert<ModuleInfoData>()) {
         currentPlayerType = 0;
        qDebug("1234----");
        QStandardItem *item_parent = modelTreeModule->item(index.row(),1);
        ModuleInfoData moduleInfoData = v_1.value<ModuleInfoData>();

       if(index.column() == 2){
           ModuleDialog moduleDialog;
          // qDebug("13---name %s",moduleInfoData.moduleConfig.name.c_str());
          // qDebug("13cid %d",moduleInfoData.moduleConfig.cidArray);
          // qDebug("mid === %d",moduleInfoData.moduleConfig.mid);
           moduleDialog.setModuleConfig(moduleInfoData.moduleConfig,&moduleDialog);
           moduleDialog.exec();


          // ErrorInfo e;
           ModuleInfoData mInfoData;
           QStandardItem* itemModule = item;

           QSettings *mConfig;
           QString mDir = QCoreApplication::applicationDirPath();
           mDir += "/module_"+ QString::number(moduleInfoData.moduleConfig.mid)+".ini";

          // qWarning("mDir is ===%s",mDir.toStdString().c_str());
           mConfig = new QSettings(mDir,QSettings::IniFormat);

           mInfoData.moduleConfig.name = mConfig->value("/description/name").toString().toStdString();
           mInfoData.moduleConfig.alias = mConfig->value("/description/alias").toString().toStdString();
           mInfoData.moduleConfig.ip = mConfig->value("/network/ip").toString().toStdString();
           //    mcrClient->getModuleConfig(mInfoData.moduleConfig,moduleInfoData.moduleConfig.mid,moduleServerUrl.c_str(),e);

            itemModule->setData(QVariant::fromValue(moduleInfoData));
            item_parent->setText(trUtf8(mInfoData.moduleConfig.name.c_str()));

        return ;
       }
    }
    if(item->parent() == NULL){
        return;
    }
    //---------------------------------
    QStandardItem* item0 = item->parent()->child(index.row());
    QVariant v = item0->data(Qt::UserRole+1);
    if(v.isNull()){
        qWarning("tree no data:index:%d,%d\n",index.row(),index.column());
    }
    if(v.canConvert<ChannelInfoData>()){
        currentPlayerType = 1;
        if(index.column()==2){
            ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
            if(channelInfoData.streamOnline == 0){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道未在线"),QMessageBox::Ok);
                return;
            }else if(channelInfoData.recordOnline == 0){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("文件服务器未在线"),QMessageBox::Ok);
                return;
            }
            //get channel config for channeldialog


            ChannelDialog channelDialog;
            channelDialog.setChannelConfig(channelInfoData.channelConfig,&channelDialog);
            channelDialog.exec();
            //--------------------
            ChannelConfig cConf;
            ErrorInfo e;
            //获取当前模块的IP
            QString aDir = QCoreApplication::applicationDirPath();
            aDir += "/module_"+ QString::number(channelInfoData.channelConfig.mid) +".ini";
            qWarning(aDir.toStdString().c_str());
            config = new QSettings(aDir,QSettings::IniFormat);

             std::string moduleServerUrl;

            moduleServerUrl = "http://";
            moduleServerUrl += config->value("/network/ip").toString().toStdString();
            moduleServerUrl +=  "/soapserver.php";
            mcrClient->getChannelConfig(cConf,channelInfoData.channelConfig.mid,channelInfoData.channelConfig.cid,moduleServerUrl.c_str(),e);

            QStandardItem* item1 = item->parent()->child(index.row(),1);
            QStandardItem* item2 = item->parent()->child(index.row(),2);

            std::string contentStr;
            contentStr.append(cConf.alias.c_str());
            contentStr.append("\r\n");
            contentStr.append(cConf.streamUrl.c_str());

            item1->setText(QObject::tr(contentStr.c_str()));
            item1->setData(QVariant::fromValue(channelInfoData));
            item1->setTextAlignment(Qt::AlignLeft);
            item2->setData(QVariant::fromValue(channelInfoData));
            item0->setData(QVariant::fromValue(channelInfoData));
            //--------------------

    qDebug()<<"on click tree1";
            return;
        }

        ChannelInfoData channelInfoData = v.value<ChannelInfoData>();

        if(channelInfoData.streamOnline == 0){
            qDebug()<<"offline:mid,cid"<<channelInfoData.channelConfig.mid<<channelInfoData.channelConfig.cid;
            //QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道未在线"),QMessageBox::Ok);
            qDebug()<<"on click tree2";
            return;
        }else if(channelInfoData.recordOnline == 0){
            // QMessageBox::information(this,QObject::tr("提示"),QObject::tr("文件服务器未在线"),QMessageBox::Ok);
            qDebug()<<"on click tree3";
            return;
        }
        fileInfoList.clear();


        if(currentChannelConfig){
            delete currentChannelConfig;
        }
        currentChannelConfig = new ChannelConfig;
        *currentChannelConfig = channelInfoData.channelConfig;
        ui->tableView_File->clearSelection();
        alertLabel->setText("Loading ...... ");
        alertDlg->resize(400,200);
        alertDlg->show();

        qDebug()<<"checkFileThread start";
        CheckFileListThreadParam* param = new CheckFileListThreadParam;
        param->dlg = this;
        param->item = item0;
        WorkerThread* checkFileListThread = NULL;
        checkFileListThread = new WorkerThread(param,checkFileList);
        checkFileListThread->setStackSize(1*1024*1024);
        checkFileListThread->start();
        while(checkFileListThread->isRunning()){
            QApplication::processEvents();
        }
        qDebug("checkFileListThread quit while() %p", checkFileListThread);
        qDebug()<<checkFileListThread->isFinished();
        delete checkFileListThread;
        qDebug("checkFileListThread has delete");
        delete param;
        qDebug("param has delete");
        alertDlg->hide();
        currentPageNo = 1;

        filterFileInfoList();
        updateFileTable();
        ui->dateEdit_Start->setDate(QDate::fromString("2000-01-01"));
        ui->dateEdit_Stop->setDate(QDate::currentDate());
        //---------------------------------
        //        int j;
        //        for(j=0;j<pageSize;j++){
        //              QStandardItem* item5 = modelTableFile->item(j,5);;
        //              item5->setText("");
        //        }
        //-------------------------------------
    }
    qDebug()<<"on click tree4";
    return;

}
/*
 * Function: asyncGetChannelFileList()
 * Description: the function is to get a file list from server.
 * Calls:
 *  1.getChannelFileList();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::asyncGetChannelFileList(){
    ErrorInfo e;
    e.result = 0;
    mcrClient->getChannelFileList(
                currentChannelConfig->mid,
                currentChannelConfig->cid,
                fileInfoList,
                currentChannelConfig->recordServerUrl.c_str(),
                e);
    if(e.result == -256){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("获取文件列表超时！"));
    }
    qWarning("getChannelFileList successful\n");
    return;
}

/*
 * Function: asyncDeleteChannelFileList()
 * Description: the function is to delete the file which selected from the tablelist.
 * Calls:
 *  1.deleteChannelFile();
 *  2.getChannelFileList();
 *  3.filterFileInfoList();
 *  4.updateFileTable();
 * Input: indexList
 * Output:none
 * Return:none
 */
void Dialog::asyncDeleteChannelFileList(QModelIndexList indexList){
    QModelIndex index;
    qWarning("tableView_File indexList:%d\n",indexList.size());
    bool succeed = false;
    foreach (index, indexList) {
  qWarning("index row is %d ",index.row());
      //  qWarning("index column is %d",index.column());
        if(index.column()!=0){
            continue;
        }

        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<FileInfoData>()){
            ErrorInfo e;
            FileInfoData fid = v.value<FileInfoData>();
            qDebug() << "deleteChannelFile--------0";
            mcrClient->deleteChannelFile(fid.mid,fid.cid,fid.fi.localUrl,currentChannelConfig->fileServerUrl.c_str(),e);
            qDebug() << "deleteChannelFile--------1";
            if(e.result == 0){
                succeed = true;
            }

        }else{
            break;
        }
        if(!succeed){
            break;
        }
    }
    if(!succeed){
        return;
    }
qDebug() << "foreach2--------0";
    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<FileInfoData>()){

            FileInfoData fid = v.value<FileInfoData>();
qDebug() << "vconvert--------0";

            std::vector<FileInfo>::iterator it;
            for(it=fileInfoList.begin();it!=fileInfoList.end();it++){

                if(it->localUrl == fid.fi.localUrl ){
                    localUrl = fid.fi.localUrl;//set flag of delete file
                    it = fileInfoList.erase(it);

                    FileInfoData fileInfoData;
                    int currentCow,totalCow;
                    currentCow =index.row() ;
                    totalCow = (filteredFileInfoList.size() - 1) % pageSize;

                    if(currentCow == totalCow) {
//                        QStandardItem* item5 = modelTableFile->item(currentCow,5);;
//                        item5->setData(QVariant::fromValue(fileInfoData));
//                        item5->setText("");
                    }else{
//                        QStandardItem* item5 = modelTableFile->item(totalCow,5);;
//                        item5->setData(QVariant::fromValue(fileInfoData));
//                        item5->setText("");
                    }
                    filterFileInfoList();
                    break;
                }
            }
        }else{
            break;
        }

    }
qDebug() << "getchannelfilelist--------";
    if(succeed){
        ErrorInfo e;
        fileInfoList.clear();
        mcrClient->getChannelFileList(
                    currentChannelConfig->mid,
                    currentChannelConfig->cid,
                    fileInfoList,
                    currentChannelConfig->recordServerUrl.c_str(),
                    e);

    }


    //更新页数显示
//-----------------------------------
    int totalPage = ((filteredFileInfoList.size()-1)/pageSize)+1;
    int lastPageRow = (filteredFileInfoList.size() - 1) % pageSize + 1;

    if(currentPageNo == totalPage + 1 && lastPageRow == 10) {
        int newPageNo = currentPageNo - 1;
        totalPage = ((filteredFileInfoList.size()-1)/pageSize)+1;
        if(newPageNo<1||newPageNo>totalPage){
            return;
        }
        currentPageNo = newPageNo;      
    }
//-------------------------------------
    filterFileInfoList();
    updateFileTable();

    return;
}

void Dialog::asyncDeleteChannelAllFile(QModelIndexList indexList)
{
    QModelIndex index ;
    QStandardItem* item  = NULL;
    qWarning("tableView_File indexList:%d\n",indexList.size());

    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        item = modelTreeModule->itemFromIndex(index);
        QVariant v = index.data(Qt::UserRole+1);

        if(v.canConvert<ChannelInfoData>()){
            ErrorInfo e;
            ChannelInfoData cInfoData = v.value<ChannelInfoData>();

            cInfoData.pro = new QProcess(this);
            cInfoData.isDelete = 1;
           connect(cInfoData.pro, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finish_process(int, QProcess::ExitStatus)));
           connect(cInfoData.pro, SIGNAL(started()), this, SLOT(start_process()));

            item->setData(QVariant::fromValue(cInfoData));
            QStringList str;
            str.clear();
            str.append(QString::number(cInfoData.channelConfig.mid));
            str.append(QString::number(cInfoData.channelConfig.cid));
            str.append(QString(QString::fromLocal8Bit(cInfoData.channelConfig.fileServerUrl.c_str())));
            QString exe  = QApplication::applicationDirPath().append("/MCRClientDeleteAllFile.exe");
            cInfoData.pro->start(exe,str);
            //cInfoData.pro->startDetached(exe,str);
            //cInfoData.pro->execute(exe,str);
        }
    }

}
/*
 * Function: asyncDeleteChannel()
 * Description: the function is to delete channel you want .
 * Calls:
 *  1.deleteChannel();
 * Input: indexList
 * Output:none
 * Return:none
 */
void Dialog::asyncDeleteChannel(QModelIndexList indexList)
{
    QModelIndex index;
    qWarning("tableView_File indexList:%d\n",indexList.size());

     ChannelInfoData cInfoData;
    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        QVariant v = index.data(Qt::UserRole+1);

        if(v.canConvert<ChannelInfoData>()){
            ErrorInfo e;
            cInfoData = v.value<ChannelInfoData>();
            qDebug("mid = %d,cid = %d",cInfoData.channelConfig.mid,cInfoData.channelConfig.cid);

            //get module ip
            std::string tmp_serverUrl = "";
            std::string serverUrl = "";
            QString aDir = QCoreApplication::applicationDirPath();
            aDir += "/module_"+QString::number(cInfoData.channelConfig.mid)+".ini";
            qWarning(aDir.toStdString().c_str());

            config = new QSettings(aDir,QSettings::IniFormat);
            tmp_serverUrl = config->value("/network/ip").toString().toStdString();

            serverUrl = "http://"+tmp_serverUrl +"/soapserver.php";
            qWarning("tmp_serverUrl %s",tmp_serverUrl.c_str());

            mcrClient->deleteChannel(cInfoData.channelConfig.mid,cInfoData.channelConfig.cid,serverUrl.c_str(),e);
            if(e.result != 0){

            }else {

            }
            mcrClient->deleteChannelRecord(cInfoData.channelConfig.mid,cInfoData.channelConfig.cid,serverUrl.c_str(),e);
            if(e.result != 0){

            }else {

            }
        }
    }
    return;
}
/*
 * Function: asyncDeleteModule()
 * Description: the function is to delete module you want .
 * Calls:
 *  1.deleteModule();
 * Input: indexList
 * Output:none
 * Return:none
 */
void Dialog::asyncDeleteModule(QModelIndexList indexList)
{
    QModelIndex index;
    qWarning("tableView_File indexList:%d\n",indexList.size());
    //bool succeed = true;
    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<ModuleInfoData>()){

            ModuleInfoData mInfoData = v.value<ModuleInfoData>();

            qDebug("mid = %d",mInfoData.moduleConfig.mid);



        //------------------------
        //update file module_count.ini
        deleteMid(mInfoData.moduleConfig.mid);

        //-------------------------
            QString mDir = QCoreApplication::applicationDirPath();
            mDir += "/module_"+QString::number(mInfoData.moduleConfig.mid)+".ini";

            if(QFile(mDir).exists()){
                QFile(mDir).remove();
            }

        }
    }

    return;
}

/*
 * Function: on_pushButton_DownloadStart_clicked()
 * Description: this is a slot function.if you want to download a file ,first you selete
 * the file and then you can click the download Button,the function will be called and excute
 * the download action.
 * Calls:
 *  1.getFileFromURL();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_DownloadStart_clicked()
{
    ErrorInfo e;
#ifdef MYFFPLAYER
    bool isPlaying = -1 ;
    isPlaying = mPlayer->isPlaying();
    if(isPlaying){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请停止播放后下载文件"));
        return;
    }
#else
    int isPlaying = -1 ;
    mcrClient->getStatusIsPlayingPlayer(isPlaying,ipcUrl,e);
    mPlayer->isPlaying();
    if(1 == isPlaying){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请停止播放后下载文件"));
        return;
    }
#endif
    QModelIndexList indexList = ui->tableView_File->selectionModel()->selectedIndexes();
    QModelIndex index;
    qWarning("tableView_File indexList:%d\n",indexList.size());

    if(indexList.isEmpty()){
        return;
    }
    int tmp_mid = -1;
    int tmp_cid = -1;

    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<FileInfoData>()){
            FileInfoData fileInfoData = v.value<FileInfoData>();
            tmp_mid = fileInfoData.mid;
            tmp_cid = fileInfoData.cid;
            break;
        }
    }

    //-----------------------------
    //judge  delete the channel all file.
    if(tmp_mid != -1){
                QModelIndex index0 = modelTreeModule->index(tmp_mid,0,QModelIndex());
                QModelIndex index1 = modelTreeModule->index(tmp_cid,0,index0);

                QStandardItem *item = modelTreeModule->itemFromIndex(index1);

                QVariant v1 = item->data(Qt::UserRole+1);

                if(v1.canConvert<ChannelInfoData>()){
                    ChannelInfoData channelInfoData = v1.value<ChannelInfoData>();

                    if(channelInfoData.isDelete == 1){
                        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道文件正在被删除,不能下载该文件！"));
                        return;
                    }
                    if(channelInfoData.streamStatus == StreamStatusRunning&&channelInfoData.recordStatus == RecordStatusRunning){
                        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道文件正在录制中，请暂停录制后下载文件"));
                        return;
                    }

                }
    }
    //------------------------------

    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<FileInfoData>()){
            FileInfoData fileInfoData = v.value<FileInfoData>();


            std::string localPath1 = getLocalPath(fileInfoData);
            int pos1 = fileInfoData.fi.localUrl.find_last_of('/');
            localPath1.append(fileInfoData.fi.localUrl.substr(pos1));
            QFileInfo info1(localPath1.c_str());

            if(fileInfoData.fi.size == info1.size()){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该文件已下载"),QMessageBox::Ok);
                continue;
            }

            std::map<std::string,Download*>::iterator it;
            it = downloadMap.find(fileInfoData.fi.downloadUrl);
            if(it!=downloadMap.end()){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("已经在下载队列中"),QMessageBox::Ok);
                continue;
            }

            Download* d = new Download;
            std::string localPath = getLocalPath(fileInfoData);
            QDir dir;
            if(!dir.exists(localPath.c_str())){
                dir.mkpath(localPath.c_str());
            }
            int pos = fileInfoData.fi.localUrl.find_last_of('/');
            localPath.append(fileInfoData.fi.localUrl.substr(pos));
            QString localFile(localPath.c_str());
            QString url(fileInfoData.fi.downloadUrl.c_str());

                d->getFileFromURL(url,localFile,fileInfoData.fi.size);

                downloadMap.insert(std::pair<std::string,Download*>(fileInfoData.fi.downloadUrl,d));

        }
    }


}
/*
 * Function: on_pushButton_DownloadStop_clicked()
 * Description: this is a slot function.you can stop the download process.then the file
 * will be change his download status.
 * Calls:
 *  1.stopDownload();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_DownloadStop_clicked()
{
    QModelIndexList indexList = ui->tableView_File->selectionModel()->selectedIndexes();
    QModelIndex index;
    qWarning("tableView_File indexList:%d\n",indexList.size());
    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
       // QStandardItem* item  = modelTableFile->item(index.row(),index.column());
        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<FileInfoData>()){
            FileInfoData fileInfoData = v.value<FileInfoData>();
            std::map<std::string,Download*>::iterator it;
            it = downloadMap.find(fileInfoData.fi.downloadUrl);
            if(it==downloadMap.end()){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("不在下载队列中"),QMessageBox::Ok);
                continue;
            }
            Download* d = it->second;
            d->stopDownload();
            downloadMap.erase(it);
            delete d;
        }
    }


}
/*
 * Function: on_pushButton_DeleteFile_clicked()
 * Description: this is a slot function.you can delete a file seleted,when you clicked
 * delete button.the function will call a pthread to excute the delete process.
 * Calls:
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_DeleteFile_clicked()
{
    ErrorInfo e;

#ifdef MYFFPLAYER
    bool isPlaying = -1 ;
    isPlaying = mPlayer->isPlaying();
    if(isPlaying){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请停止播放后执行删除操作"));
        return;
    }
#else
    int isPlaying = -1 ;
    mcrClient->getStatusIsPlayingPlayer(isPlaying,ipcUrl,e);
    if(1 == isPlaying){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请停止播放后执行删除操作"));
        return;
    }
#endif
    QModelIndexList indexList = ui->tableView_File->selectionModel()->selectedIndexes();
    QModelIndex index;

    if(indexList.isEmpty()){
        return;
    }
    foreach (index, indexList) {
        if(index.column()!=0){
             continue;
        }

    QVariant v = index.data(Qt::UserRole+1);
    if(v.canConvert<FileInfoData>()){
        FileInfoData fileInfoData;
        fileInfoData = v.value<FileInfoData>();
        fileInfoData.downloading = false;
         //-----------------------------
         //judge  delete the channel all file.
                     QModelIndex index0 = modelTreeModule->index(fileInfoData.mid,0,QModelIndex());
                     QModelIndex index1 = modelTreeModule->index(fileInfoData.cid,0,index0);

                     QStandardItem *item = modelTreeModule->itemFromIndex(index1);

                     QVariant v1 = item->data(Qt::UserRole+1);

                     if(v1.canConvert<ChannelInfoData>()){
                         ChannelInfoData channelInfoData = v1.value<ChannelInfoData>();
                         if(channelInfoData.isDelete == 1){
                             QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道文件正在被删除"));
                             continue;
                         }

                     }
         //------------------------------
        if(fileInfoData.fi.httpURL == httpUrl){
              QMessageBox::information(this,QObject::tr("提示"),QObject::tr("当前文件正在播放"));
              return ;       
        }
        if(fileInfoData.downloading){
            QMessageBox::information(this,QObject::tr("提示"),QObject::tr("当前文件正在下载"));
            return ;
        }
    }
    }


    if(QMessageBox::No == QMessageBox::information(this,QObject::tr("提示"),
                                                    QObject::tr("请确认是否要删除当前文件"),
                                                    QMessageBox::Yes|QMessageBox::No,
                                                    QMessageBox::Yes)){
        return;
    }


    alertLabel->setText(QObject::tr("正在删除文件，请稍候 ...... "));
    alertDlg->resize(400,200);
    alertDlg->show();

    DeleteFileListThreadParam* param = new DeleteFileListThreadParam;
    param->dlg = this;
    param->indexList = indexList;
   WorkerThread* deleteFileListThread = new WorkerThread(param,deleteFileList);
    deleteFileListThread->setStackSize(1*1024*1024);
    deleteFileListThread->start();
    while(deleteFileListThread->isRunning()){
        QApplication::processEvents();
    }
    qDebug("deleteFileListThread quit while()");
    delete deleteFileListThread;
    qDebug("deleteFileListThread have delete");
    delete param;
    qDebug("param have delete");
    alertDlg->hide();
    ui->tableView_File->clearSelection();
}
/*
 * Function: on_pushButton_PageUp_clicked()
 * Description: this is a slot function.when you click the previous page button,the
 * function will be called.
 * Calls:
 * 1.updateFileTable();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_PageUp_clicked()
{

    //-----------------------------
    //judge  delete the channel all file.
    if(currentChannelConfig == NULL){
        return;
    }
                QModelIndex index0 = modelTreeModule->index(currentChannelConfig->mid,0,QModelIndex());
                QModelIndex index = modelTreeModule->index(currentChannelConfig->cid,0,index0);

                QStandardItem *item = modelTreeModule->itemFromIndex(index);

                QVariant v = item->data(Qt::UserRole+1);

                if(v.canConvert<ChannelInfoData>()){
                    ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
                    if(channelInfoData.isDelete == 1){
                        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道文件正在被删除"));
                        return;
                    }

                }
    //------------------------------
    int newPageNo = currentPageNo - 1;
    int totalPage = ((filteredFileInfoList.size()-1)/pageSize)+1;
    if(newPageNo<1||newPageNo>totalPage){
        return;
    }
    ui->tableView_File->clearSelection();
    currentPageNo = newPageNo;
    updateFileTable();
}
/*
 * Function: on_pushButton_PageDown_clicked()
 * Description: this is a slot function.when you click the next page button,the
 * function will be called.
 * Calls:
 * 1.updateFileTable();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_PageDown_clicked()
{
    //-----------------------------
    //judge  delete the channel all file.
    if(currentChannelConfig == NULL){
        return;
    }
                QModelIndex index0 = modelTreeModule->index(currentChannelConfig->mid,0,QModelIndex());
                QModelIndex index = modelTreeModule->index(currentChannelConfig->cid,0,index0);

                QStandardItem *item = modelTreeModule->itemFromIndex(index);

                QVariant v = item->data(Qt::UserRole+1);

                if(v.canConvert<ChannelInfoData>()){
                    ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
                    if(channelInfoData.isDelete == 1){
                        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道文件正在被删除"));
                        return;
                    }

                }
    //------------------------------
    int newPageNo = currentPageNo + 1;
    int totalPage = ((filteredFileInfoList.size()-1)/pageSize)+1;
    if(newPageNo<1||newPageNo>totalPage){
        return;
    }
    ui->tableView_File->clearSelection();
    currentPageNo = newPageNo;
//==============================

    int tmp = 0,tmp1 = 0,tmp2 = 0;
    int j = 0;

    tmp = pageSize ;
    tmp1 = (filteredFileInfoList.size()) % pageSize;

    if(currentPageNo == totalPage ) {
        if(tmp1 == 0) {
            tmp2 = tmp;
        }else {
            tmp2 = tmp1;
        }
//        for(j=tmp2;j<pageSize;j++){
//            FileInfoData fileInfoData;
//            QStandardItem* item5 = modelTableFile->item(j,5);;
//            item5->setData(QVariant::fromValue(fileInfoData));
//            item5->setText("");
//        }
     }
    updateFileTable();
}
/*
 * Function: on_tableView_File_clicked()
 * Description:the function is to set a flag about play source. when you click
 * play button ,it will use the currentPlayerType variable.
 * Calls:
 * Input: index
 * Output:none
 * Return:none
 */
void Dialog::on_tableView_File_clicked(const QModelIndex &index)
{
    if(!index.isValid()){
        return;
    }
    currentPlayerType = 0;
    QModelIndex idx = index.model()->index(index.row(),0);

    QVariant v = idx.data(Qt::UserRole+1);
    if(v.isNull()){
        qWarning("no data:index:%d,%d\n",index.row(),index.column());

    }
    if(v.canConvert<FileInfoData>()){
        currentPlayerType = 2;
       }

}
/*
 * Function: on_tableView_File_doubleClicked()
 * Description:when you double clicked a file from tableList,it will play the video
 * resource .
 * Calls:
 * Input: index
 * Output:none
 * Return:none
 */
/*
void Dialog::on_tableView_File_doubleClicked(const QModelIndex &index)
{

}
*/

/*
 * Function: updatePlayerUI()
 * Description:the function is to update the play status.eg:show video totaltime and
 * currentTime.
 * Calls:none
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::updatePlayerUI(){
    if(flagLive == 1){
        ui->horizontalSlider_Player->setDisabled(true);
    }else if(flagLive == 0){
       // ui->horizontalSlider_Player->setEnabled(false);
    }
    ;

#ifdef MYFFPLAYER
    bool isOpened = false,isPlaying = false ,canSeek = false;
    isOpened = mPlayer->isOpened();
    if(!isOpened){
        ui->label_PlayerTime->setText("");
        ui->label_PlayerLength->setText("");
       // ui->label_PlayerInfo->setText("");
        ui->graphicsView_Player->clear();
    }else{

    }
    isPlaying = mPlayer->isPlaying();
#else
    int isOpened = -1 ,isPlaying = -1,canSeek = -1;

    ErrorInfo e;
    mcrClient->getStatusIsOpenedPlayer(isOpened,ipcUrl,e);

    if(1 != isOpened){
        ui->label_PlayerTime->setText("");
        ui->label_PlayerLength->setText("");
       // ui->label_PlayerInfo->setText("");
        ui->graphicsView_Player->clear();
    }else{

    }
    mcrClient->getStatusIsPlayingPlayer(isPlaying,ipcUrl,e);
#endif



    if(1 == isPlaying){
            if(flagLive == 0){
                ui->horizontalSlider_Player->setEnabled(true);
            }


            int64_t length = 0 ,time = 0;
#ifdef MYFFPLAYER
            length = mPlayer->length();
            time = mPlayer->time();
#else
            mcrClient->getFileLengthPlayer(length,ipcUrl,e);
            mcrClient->getFileTimePlayer(time,ipcUrl,e);
#endif
          //  qWarning("length is %lld,time is %lld\n",length,time);
            if(time>0){
                QTime n(0,0,0,0);
                QTime t;
                QString timeStr = "" ;
                t = n.addSecs(time);
                 // if(flagLive == 0){
                    ui->label_PlayerTime->setText(t.toString("hh:mm:ss"));
                    timeStr = ui->label_PlayerTime->text();
                  //}
                    qWarning("current process %s",timeStr.toStdString().c_str());
                  if(0 == timeStr.compare("00:00:04")){
                      ui->tableView_File->setDisabled(false);
                  }

            }
            if(length>0){
                QTime n(0,0,0,0);
                QTime t;
                t = n.addSecs(length);
                //如果非直播，则不显示时长和播放进度
                if(flagLive == 0){
                    ui->label_PlayerLength->setText(t.toString("hh:mm:ss"));
                }
            }

                float ratio = (float)time / length;
                //qWarning("lenth is %lld, current position is %lld,ratio is %f",length,time,ratio);
                if(ui->horizontalSlider_Player->isEnabled()){
                    ui->horizontalSlider_Player->setValue(ratio*1000);
                }
                qWarning("slider value is %d,%lld,%lld",ui->horizontalSlider_Player->value(),time,length);
                if(length == time  && time > 0 && length > 0){
                     ui->horizontalSlider_Player->setDisabled(true);
                     ui->horizontalSlider_Player->setValue(0);
                     ui->label_PlayerTime->setText("");
                     ui->label_PlayerLength->setText("");
                     ui->graphicsView_Player->clear();

                     if(fullScreen && flagLive == 0){
                         ui->groupBox_Left->show();
                         ui->widget_File->show();
                         ui->tableView_File->show();
                         ui->widget_Player->show();

                         showMaximized();
                         ui->graphicsView_Player->setGeometry(showVedioRect);
                         fullScreen = false;
                     }
                     on_pushButton_PlayerStop_clicked();
                }



    }else if((1 == isOpened) && (1 != isPlaying)){

    }else if(1 != isOpened){
        //关闭播放时,处理内容
      //  if(player->position() > 0.5){
        ui->horizontalSlider_Player->setValue(0);
        ui->label_PlayerTime->setText("");
        ui->label_PlayerLength->setText("");

        playing_mid = -1;
        playing_cid = -1;
        //---------------------------
        ui->horizontalSlider_Player->setEnabled(false);

        if(fullScreen){
            ui->groupBox_Left->show();
            ui->widget_File->show();
            ui->tableView_File->show();
            ui->widget_Player->show();
          //  ui->verticalLayout->setMargin(5);
           // ui->graphicsView_Player->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
            showMaximized();
            ui->graphicsView_Player->setGeometry(showVedioRect);
            fullScreen = false;
        }
       // }
        //----------------------------
    }

}
/*
 * Function: on_pushButton_PlayerStart_clicked()
 * Description:this is a slot function.when you click the play button,you can play the
 * vedio resource you selected.
 * Calls:none
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_PlayerStart_clicked()
{

    QString aDir = QCoreApplication::applicationDirPath();
    aDir += "/config.ini";

    config = new QSettings(aDir,QSettings::IniFormat);
    QString videoWidth = config->value("/system/videoWidth").toString();
    if(videoWidth == ""){
        VEDIOWIDTH = 0;
    }else{
        VEDIOWIDTH = videoWidth.toInt();
    }
    QString videoHeight = config->value("/system/videoHeight").toString();
    if(videoHeight == ""){
        VEDIOHEIGHT = 0;
    }else {
        VEDIOHEIGHT = videoHeight.toInt();
    }
    QString fullVideoWidth = config->value("/system/fullVideoWidth").toString();
    if(fullVideoWidth == ""){
        fVEDIOWIDTH = 0;
    }else{
        fVEDIOWIDTH = fullVideoWidth.toInt();
    }
    QString fullVideoHeight= config->value("/system/fullVideoHeight").toString();
    if(fullVideoHeight == ""){
        fVEDIOHEIGHT = 0;
    }else{
        fVEDIOHEIGHT = fullVideoHeight.toInt();
    }

    QString screenWidth = config->value("/system/screenWidth").toString();
    if(fullVideoWidth == ""){
        SCREENWIDTH = 1600;
    }else{
        SCREENWIDTH = screenWidth.toInt();
    }
    QString screenHeight= config->value("/system/screenHeight").toString();
    if(screenHeight == ""){
        SCREENHEIGHT = 1200;
    }else{
        SCREENHEIGHT = screenHeight.toInt();
    }

//    QGraphicsScene *scene = new QGraphicsScene;
    showVedioRect  = ui->graphicsView_Player->geometry();

    if(currentPlayerType == 1){
        ErrorInfo e;
#ifdef MYFFPLAYER
        bool isOpened = false;
        bool isPlaying = false;
#else
        int isOpened = -1;
        int isPlaying = -1;
#endif
        QModelIndexList indexList = ui->treeView_Module->selectionModel()->selectedIndexes();
        QModelIndex index;
        foreach (index, indexList) {
            if(index.column()!=0){
                continue;
            }

            QVariant v = index.data(Qt::UserRole+1);
            if(v.canConvert<ChannelInfoData>()){
                ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
                if(channelInfoData.streamStatus == StreamStatusStopped){
                    QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道无视频信号源"),QMessageBox::Ok);
                    continue;
                }


                DBG(DBG_CRITICAL,"you choose live vedio\n");
                QString url = QString(QString::fromLocal8Bit(channelInfoData.channelConfig.streamUrl.c_str()));
#ifdef MYFFPLAYER
                isOpened  = mPlayer->isOpened();
#else
                 mcrClient->getStatusIsOpenedPlayer(isOpened,ipcUrl,e);
#endif
                 //如果两个播放地址不同，则关掉，重新播放
                 if(0 != url_bak.compare(url)){
#ifdef MYFFPLAYER
                    isPlaying = mPlayer->isPlaying();
#else
                     mcrClient->getStatusIsPlayingPlayer(isPlaying,ipcUrl,e);
#endif
                     if(isPlaying){
                        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请先停止播放"));
                        return;
                     }
                      if(isOpened){
#ifdef MYFFPLAYER
                          mPlayer->close();
#else
                          mcrClient->closePlayer(ipcUrl,e);
#endif

                      }
                      ui->label_PlayerTime->setText("");
                      ui->label_PlayerLength->setText("");
                      // player->setPosition(0.0);
                     // ui->horizontalSlider_Player->setValue(0);

                  }
                 if(1 != isOpened){
#ifdef MYFFPLAYER
                   bool  ret = mPlayer->open(url,ui->graphicsView_Player->winId(),true);
                    if(ret){
#else
                     mcrClient->openPlayer(&url,ipcUrl,1,e);
                     if(e.result == 0){
#endif


                        DBG(DBG_CRITICAL,"you open the vedio\n");
                        url_bak = url;
#ifdef MYFFPLAYER
                        mPlayer->play();
                        if(showVedioes != 0){
                         // qWarning(" kill in live function showVedioes value   is %d\n",showVedioes);
                          killTimer(showVedioes);
                          showVedioes = 0;
                        }
                        showVedioes = startTimer(20);
#else
                        mcrClient->playFilePlayer(ipcUrl,e);
                        DBG(DBG_CRITICAL,"you play the vedio\n");
                        //从共享内存中获取视频信息
                        if(showVedioes != 0){
                         // qWarning(" kill in live function showVedioes value   is %d\n",showVedioes);
                          killTimer(showVedioes);
                          showVedioes = 0;
                        }
                        showVedioes = startTimer(20);
                       // qWarning("in live function showVedioes value   is %d\n",showVedioes);
                        DBG(DBG_CRITICAL,"you boot the showVedioes timer\n");
#endif

                       // player->play();
                    }
                    if(playerTimer!=0){
                        killTimer(playerTimer);
                        playerTimer = 0;
                    }

                    playerTimer = startTimer(300);
                    DBG(DBG_CRITICAL,"you boot the playerTimer timer\n");
                }
//                if(player->isOpened()&&!player->isPlaying()){
//                    player->play();
//                }

                 //disable 进度条
                 if(ui->horizontalSlider_Player->isEnabled()){
                     ui->horizontalSlider_Player->setDisabled(true);
                 }
                 //设置进度条为disable,定时器更新实现
                 flagLive = 1;


                std::string info("直播:");
                info.append(channelInfoData.channelConfig.alias);
                ui->label_PlayerInfo->setStyleSheet(
                            "color:white;"
                            );
                ui->label_PlayerInfo->setText(QObject::tr(info.c_str()));
                ui->label_PlayerInfo->setAlignment(Qt::AlignCenter);

                break;
            }

        }

    }else if(currentPlayerType == 2){
        ErrorInfo e;
#ifdef  MYFFPLAYER
        bool isOpened = false, isPlaying = false;
#else
        int isOpened = -1,isPlaying = -1;
#endif
        QModelIndexList indexList = ui->tableView_File->selectionModel()->selectedIndexes();
        QModelIndex index;
        foreach (index, indexList) {
            if(index.column()!=0){
                continue;
            }

            QVariant v = index.data(Qt::UserRole+1);
            if(v.canConvert<FileInfoData>()){
                FileInfoData fileInfoData = v.value<FileInfoData>(); 
                //judge  delete the channel all file.
                //-----------------------------
                 QModelIndex index0 = modelTreeModule->index(fileInfoData.mid,0,QModelIndex());
                 QModelIndex index1 = modelTreeModule->index(fileInfoData.cid,0,index0);

                 QStandardItem *item = modelTreeModule->itemFromIndex(index1);

                 QVariant v1 = item->data(Qt::UserRole+1);

                 if(v1.canConvert<ChannelInfoData>()){
                       ChannelInfoData channelInfoData = v1.value<ChannelInfoData>();
                       if(channelInfoData.isDelete == 1){
                           QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道文件正在被删除,不能播放该文件!"));
                           return;
                       }

                 }
                //------------------------------

              DBG(DBG_CRITICAL,"you choose recorded vedio\n");


                QString url = QString(QString::fromLocal8Bit(fileInfoData.fi.httpURL.c_str()));
#ifdef MYFFPLAYER
                isOpened = mPlayer->isOpened();
#else
                mcrClient->getStatusIsOpenedPlayer(isOpened,ipcUrl,e);
#endif
                //如果两个播放地址不同，则关掉，重新播放
                if(0 != url_bak.compare(url)){
                    //如果两个地址不同，则请先停止播放

#ifdef MYFFPLAYER
                    isPlaying = mPlayer->isPlaying();
#else
                    mcrClient->getStatusIsPlayingPlayer(isPlaying,ipcUrl,e);
#endif
                    if(isPlaying){
                       QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请先停止播放"));
                       return;
                    }

                      if(1 == isOpened){
#ifdef MYFFPLAYER
                          mPlayer->close();
#else
                          mcrClient->closePlayer(ipcUrl,e);
#endif
                      }
                }


                if(1 != isOpened){
#ifdef MYFFPLAYER
                    int ret = mPlayer->open(url,ui->graphicsView_Player->winId(),false);
                    url_bak = url;
                    if(ret){
#else
                    mcrClient->openPlayer(&url,ipcUrl,0,e);
                    url_bak = url;
                    if(e.result == 0){
#endif
                    // ret = player->open(url,ui->graphicsView_Player->winId(),false);

                        DBG(DBG_CRITICAL,"you open the vedio\n");

#ifdef MYFFPLAYER
                      mPlayer->play();
                      if(showVedioes != 0){
                      //  qWarning("kill in unlive function showVedioes value   is %d\n",showVedioes);
                        killTimer(showVedioes);
                      }
                      showVedioes = startTimer(20);
#else
                      mcrClient->playFilePlayer(ipcUrl,e);
                      //从共享内存中获取视频信息

                      if(showVedioes != 0){
                      //  qWarning("kill in unlive function showVedioes value   is %d\n",showVedioes);
                        killTimer(showVedioes);
                      }
                      showVedioes = startTimer(20);
                     // qWarning("in unlive function showVedioes value   is %d\n",showVedioes);
                      DBG(DBG_CRITICAL,"you boot the showVedioes timer\n");
#endif

                    }
                }else if((1 == isOpened) && (1 != isPlaying)){
                   // player->resume_play();
                    httpUrl = fileInfoData.fi.httpURL;
                }
                playing_mid = fileInfoData.mid;
                playing_cid = fileInfoData.cid;

                if(playerTimer!=0){
                    killTimer(playerTimer);
                    playerTimer = 0;
                }
                playerTimer = startTimer(300);

                DBG(DBG_CRITICAL,"you boot the playerTimer timer\n");

//                if(player->isOpened()&&!player->isPlaying()){
//                    player->play();
//                }
                //enable 进度条
                if(!ui->horizontalSlider_Player->isEnabled()){
                    ui->horizontalSlider_Player->setDisabled(false);
                }
                //设置进度条为enable,定时器更新实现
                flagLive = 0;
                std::string info("点播:");
                int pos = fileInfoData.fi.localUrl.find_last_of('/')+1;
                info.append(fileInfoData.fi.localUrl.substr(pos).c_str());
                ui->label_PlayerInfo->setStyleSheet(
                            "color:white;"
                            );
                ui->label_PlayerInfo->setText(QObject::tr(info.c_str()));
                ui->label_PlayerInfo->setAlignment(Qt::AlignCenter);
            }
        }

    }


}
/*
 * Function: on_pushButton_PlayerStop_clicked()
 * Description:this is a slot function.when you click the stop button,you can stop play the
 * vedio resource you selected.
 * Calls:none
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_PlayerStop_clicked()
{
#ifdef MYFFPLAYER
    bool isOpened = false;
#else
    int isOpened = -1;
#endif
    ErrorInfo e;

#ifdef MYFFPLAYER
    isOpened = mPlayer->isOpened();
#else
    mcrClient->getStatusIsOpenedPlayer(isOpened,ipcUrl,e);
#endif

    if(isOpened == 1){
        ErrorInfo e;
        //flag for delete all file
        playing_mid = -1;
        playing_cid = -1;
        if(playerTimer != 0){
            killTimer(playerTimer);
            playerTimer = 0;
        }
#ifndef MYFFPLAYER
        //关掉获取共享内存的定时器
      //  qWarning("in close function showVedioes value   is %d\n",showVedioes);
        if(showVedioes != 0) {
            killTimer(showVedioes);
            showVedioes = 0;
        }
#endif

#ifdef MYFFPLAYER
        mPlayer->close();
#else
        mcrClient->closePlayer(ipcUrl,e);
#endif
        qWarning("close function has call back");
        //设置tableView为enable
        ui->tableView_File->setDisabled(false);

        ui->horizontalSlider_Player->setValue(0);
        httpUrl = "";

    }else{
        qDebug("isOpened is %d\n",isOpened);
    }
    ui->label_PlayerInfo->setText("");
    ui->horizontalSlider_Player->setValue(0);
    ui->label_PlayerTime->setText("");
    ui->label_PlayerLength->setText("");
    ui->graphicsView_Player->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    ui->graphicsView_Player->clear();


}
/*
 * Function: on_pushButton_PlayerFullscreen_clicked()
 * Description:this is a slot function.you can full screen the playing vedio when you
 * click the the FullScreen button.
 * Calls:
 * 1.showFullScreen();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_PlayerFullscreen_clicked()
{

//    QGraphicsScene *scene = new QGraphicsScene;
//        scene->addPixmap(QPixmap::fromImage(*image_null));
//    ui->graphicsView_Player->setScene(scene);


#ifdef MYFFPLAYER
    bool isOpened = false;
#else
    int isOpened = -1;
#endif
    ErrorInfo e;
#ifdef  MYFFPLAYER
    isOpened = mPlayer->isOpened();
#else
    mcrClient->getStatusIsOpenedPlayer(isOpened,ipcUrl,e);
#endif
    if(isOpened == 1&&!fullScreen){
    ui->groupBox_Left->hide();
    ui->widget_File->hide();
    ui->tableView_File->hide();
    ui->widget_Player->hide();
    ui->widget_Title->hide();
    ui->widget->hide();
    ui->graphicsView_Player->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    showFullScreen();
   // showVedioRect  = ui->graphicsView_Player->geometry();
    //设置ffmpeg中，显示图片为1600*1200分辨率
   // player->setFullscreen(true);
    fullScreen = true;
    }
}
/*
 * Function: timerEvent()
 * Description:this is a timer.the function is used for update the play status,download
 * status and channel status.
 * Calls:
 * 1.updatePlayerUI();
 * 2.updateDownload();
 * Input: event
 * Output:none
 * Return:none
 */
void Dialog::timerEvent(QTimerEvent * event){

    if(event->timerId() == playerTimer){
        updatePlayerUI();
    }else if(event->timerId() == downloadTimer){
        updateDownload();
    }else if (event->timerId() == proIsRunning){
//        updateProStatus();
    }else if (event->timerId() == showVedioes){
        asyncShowVedio();
    }else if (event->timerId() == updateStatusTimer){
        traverseTree();
    }else if(event->timerId() == updateGroupStatusTimer){
         for(int i = 0;i<MODULENUM;i++){
            if(checkGroupStatusThread[i] != NULL){
                if(!checkGroupStatusThread[i]->isRunning()){
                    checkGroupStatusThread[i]->setStackSize(1*1024*1024);
                    checkGroupStatusThread[i]->start();
                }
            }
         }
    }else if(event->timerId() == sinaleThreadUpdateStatusTimer){
//        if(checkStatusThread != NULL){
//            if(!checkStatusThread->isRunning()){
//                checkStatusThread->setStackSize(1*1024*1024);
//                checkStatusThread->start();
//            }
//        }
    }else if(event->timerId() == channelTimer){
#if 1
        int i = 0;
        for(;i<modelTreeModule->rowCount();i++){


        QStandardItem* itemModule = modelTreeModule->item(i,0);
        if(itemModule == NULL){
            continue;
        }
        int j = 0;
        for(;j<itemModule->rowCount();j++){
            QStandardItem* itemChannel = itemModule->child(j,0);
            QVariant v = itemChannel->data();
            if(v.canConvert<ChannelInfoData>()){
                ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
                    if(channelInfoData.checkStatusThread==NULL){
//                        qWarning("check status thread is NULL, %d,%d",
//                                 channelInfoData.channelConfig.mid,
//                                 channelInfoData.channelConfig.cid);
                        continue;
                    }
                    if(!channelInfoData.checkStatusThread->isRunning()){
//                        qWarning("restart check status:%d,%d\n",channelInfoData.channelConfig.mid,channelInfoData.channelConfig.cid);
                        channelInfoData.checkStatusThread->setStackSize(1*1024*1024);
                        channelInfoData.checkStatusThread->start();
                    }else{
                        qWarning("check status thread is still running, %d,%d",
                                 channelInfoData.channelConfig.mid,
                                 channelInfoData.channelConfig.cid);
                        continue;
                    }
                }
            }
        }
#else
        if(checkStatusThread==NULL){
            qWarning("check status thread is NULL");
        }else if(!checkStatusThread->isRunning()){
            checkStatusThread->start();
        }
#endif
    }


}
/*
 * Function: asyncUpdateChannelStatus()
 * Description:the function is used for update the channel status.
 * Calls:
 * 1.getChannelStreamStatus();
 * 2.getChannelRecordStatus();
 * Input: itemChannel
 * Output:none
 * Return:none
 */
void Dialog::asyncUpdateChannelStatus(ChannelConfig channelConfig){
    //mutex->lock();
    channelID chID;
    chID.mid = channelConfig.mid;
    chID.cid = channelConfig.cid;
    qWarning("mid %d,cid %d\n",chID.mid,chID.cid);
    chID.strID = QString("%1_%2").arg(chID.mid,chID.cid);
    std::map<QString,int>::iterator it;
    int streamStatus = 0,recordStatus = 0;
    ErrorInfo e;
    e.result = 0;
    e.info = "";
    struct timespec tv;
    struct timespec tv_rval;
    while(1){


                mcrClient->getChannelStreamStatus(streamStatus,channelConfig.mid,channelConfig.cid,channelConfig.streamServerUrl.c_str(),e);
               // channelInfoData.streamOnline = (e.result==-256)?0:1;
                mcrClient->getChannelRecordStatus(recordStatus,channelConfig.mid,channelConfig.cid,channelConfig.recordServerUrl.c_str(),e);
               // channelInfoData.recordOnline = (e.result==-256)?0:1;
               // qWarning("###############offline:[%d,%d] %d:%s --streamstatus%d---recordstatus%d",cc.mid, cc.cid, e.result, e.info.c_str(),streamStatus,recordStatus);

               if(streamStatus == -1){
                  qDebug("&&& mid %d, cid %d getChannelStreamStatus error",channelConfig.mid,channelConfig.cid);
                  mcrClient->getChannelStreamStatus(streamStatus,channelConfig.mid,channelConfig.cid,channelConfig.streamServerUrl.c_str(),e);
               }else if(recordStatus == -1){
                  qDebug("&&& mid %d, cid %d getChannelRecordStatus error",channelConfig.mid,channelConfig.cid);
                  mcrClient->getChannelRecordStatus(recordStatus,channelConfig.mid,channelConfig.cid,channelConfig.recordServerUrl.c_str(),e);
               }
                if(chID.mid == 0 &&chID.cid == 0)
                qDebug()<<"mid"<<chID.mid<<"cid"<<chID.cid<<"streamStatus"<<streamStatus<<"record"<<recordStatus;
                if(streamStatus == StreamStatusRunning &&
                      recordStatus == RecordStatusRunning){
//                      qWarning("%d_%d:1--1\n",channelConfig.mid,channelConfig.cid);
                     statusMapMutex->lock();
                     it = statusMap.find(chID.strID);
                      if(it != statusMap.end()){
                         // statusMap.insert(std::pair<QString,int>(chID.strID,1));
                          it->second = 1;
                      }
                     statusMapMutex->unlock();
                }else if(streamStatus == StreamStatusRunning &&
                      recordStatus == RecordStatusStopped){
//                      qWarning("%d_%d:1--0\n",channelConfig.mid,channelConfig.cid);
                      statusMapMutex->lock();
                     it = statusMap.find(chID.strID);
                      if(it != statusMap.end()){
                        //  statusMap.insert(std::pair<QString,int>(chID.strID,2));
                          it->second = 2;
                      }
                        statusMapMutex->unlock();

                }else if(streamStatus == StreamStatusStopped &&
                      recordStatus == RecordStatusStopped){
//                      qWarning("%d_%d:0--0\n",channelConfig.mid,channelConfig.cid);
                        statusMapMutex->lock();
                     it = statusMap.find(chID.strID);
                      if(it != statusMap.end()){
                         // statusMap.insert(std::pair<QString,int>(chID.strID,3));
                          it->second = 3;
                      }
                        statusMapMutex->unlock();

                }else if(streamStatus == StreamStatusStopped &&
                      recordStatus == RecordStatusRunning){
//                    qWarning("%d_%d:0--1\n",channelConfig.mid,channelConfig.cid);
                        statusMapMutex->lock();
                    it = statusMap.find(chID.strID);
                    if(it != statusMap.end()){
//                        statusMap.insert(std::pair<QString,int>(chID.strID,4));
                        it->second = 4;
                    }
                        statusMapMutex->unlock();
                }else if(streamStatus == -1 && recordStatus == RecordStatusRunning){
                    //qWarning("%d_%d:0--0\n",channelConfig.mid,channelConfig.cid);
                      statusMapMutex->lock();
                   it = statusMap.find(chID.strID);
                    if(it != statusMap.end()){
//                        statusMap.insert(std::pair<QString,int>(chID.strID,5));
                        it->second = 5;
                    }
                      statusMapMutex->unlock();
                }else if (streamStatus == -1 && recordStatus == RecordStatusStopped){
                    statusMapMutex->lock();
                  it = statusMap.find(chID.strID);
                  if(it != statusMap.end()){
//                      statusMap.insert(std::pair<QString,int>(chID.strID,6));
                      it->second = 6;
                  }
                  statusMapMutex->unlock();
                }else if(streamStatus == -1 && recordStatus == -1){
                    statusMapMutex->lock();
                   it = statusMap.find(chID.strID);
                  if(it != statusMap.end()){
//                      statusMap.insert(std::pair<QString,int>(chID.strID,7));
                      it->second = 7;
                  }
                  statusMapMutex->unlock();
                }else if(streamStatus == 0 && recordStatus == -1){
                    statusMapMutex->lock();
                   it = statusMap.find(chID.strID);
                  if(it != statusMap.end()){
//                      statusMap.insert(std::pair<QString,int>(chID.strID,8));
                      it->second = 8;
                  }
                  statusMapMutex->unlock();
                }else if(streamStatus == 1 && recordStatus == -1){
                    statusMapMutex->lock();
                   it = statusMap.find(chID.strID);
                  if(it != statusMap.end()){
//                      statusMap.insert(std::pair<QString,int>(chID.strID,9));
                      it->second = 9;
                  }
                  statusMapMutex->unlock();
                }
                //延时5秒
                if(streamStatus == -1)
                {
                    tv.tv_sec = 10;
                    tv.tv_nsec = 0;
                    nanosleep(&tv,&tv_rval);
                }else{
                    tv.tv_sec = 5;
                    tv.tv_nsec = 0;
                    nanosleep(&tv,&tv_rval);
                }
       }
}
void Dialog::asyncUpdateChannelStatusSingleThread(QStandardItem* itemChannel){

    if(itemChannel == NULL){
        return;
    }
    QVariant v = itemChannel->data();

    if(v.canConvert<ChannelInfoData>()){
        ChannelInfoData cInfoData = v.value<ChannelInfoData>();
        ChannelConfig channelConfig = cInfoData.channelConfig;
    //mutex->lock();
    channelID chID;
    chID.mid = channelConfig.mid;
    chID.cid = channelConfig.cid;
   // qWarning("mid %d,cid %d\n",chID.mid,chID.cid);
    chID.strID = QString("%1").arg(chID.mid)+"_"+QString::number(chID.cid);
    std::map<QString,int>::iterator it;
    int streamStatus = 0,recordStatus = 0;
    ErrorInfo e;
    e.result = 0;
    e.info = "";
                mcrClient->getChannelStreamStatus(streamStatus,channelConfig.mid,channelConfig.cid,channelConfig.streamServerUrl.c_str(),e);
               // channelInfoData.streamOnline = (e.result==-256)?0:1;
                mcrClient->getChannelRecordStatus(recordStatus,channelConfig.mid,channelConfig.cid,channelConfig.recordServerUrl.c_str(),e);
               // channelInfoData.recordOnline = (e.result==-256)?0:1;
               // qWarning("###############offline:[%d,%d] %d:%s --streamstatus%d---recordstatus%d",cc.mid, cc.cid, e.result, e.info.c_str(),streamStatus,recordStatus);


                if(streamStatus == StreamStatusRunning &&
                      recordStatus == RecordStatusRunning){
//                      qWarning("%d_%d:1--1\n",channelConfig.mid,channelConfig.cid);
                     statusMapMutex->lock();
                     it = statusMap.find(chID.strID);
                      if(it != statusMap.end()){
                         // statusMap.insert(std::pair<QString,int>(chID.strID,1));
                          it->second = 1;
                          qWarning("mid %d,cid %d insert into map streamStatus: %d"
                                   "recordStatus :%d ip :%s\n",
                                   chID.mid,chID.cid,streamStatus,recordStatus,
                                   channelConfig.ip.c_str()
                                   );
                      }
                     statusMapMutex->unlock();
                }else if(streamStatus == StreamStatusRunning &&
                      recordStatus == RecordStatusStopped){
//                      qWarning("%d_%d:1--0\n",channelConfig.mid,channelConfig.cid);
                      statusMapMutex->lock();
                     it = statusMap.find(chID.strID);
                      if(it != statusMap.end()){
                        //  statusMap.insert(std::pair<QString,int>(chID.strID,2));
                          it->second = 2;

                      }
                        statusMapMutex->unlock();

                }else if(streamStatus == StreamStatusStopped &&
                      recordStatus == RecordStatusStopped){
//                      qWarning("%d_%d:0--0\n",channelConfig.mid,channelConfig.cid);
                        statusMapMutex->lock();
                     it = statusMap.find(chID.strID);
                      if(it != statusMap.end()){
                         // statusMap.insert(std::pair<QString,int>(chID.strID,3));
                          it->second = 3;

                      }
                        statusMapMutex->unlock();

                }else if(streamStatus == StreamStatusStopped &&
                      recordStatus == RecordStatusRunning){
//                    qWarning("%d_%d:0--1\n",channelConfig.mid,channelConfig.cid);
                        statusMapMutex->lock();
                    it = statusMap.find(chID.strID);
                    if(it != statusMap.end()){
//                        statusMap.insert(std::pair<QString,int>(chID.strID,4));
                        it->second = 4;

                    }
                        statusMapMutex->unlock();
                }else if(streamStatus == -1 && recordStatus == RecordStatusRunning){
                    //qWarning("%d_%d:0--0\n",channelConfig.mid,channelConfig.cid);
                      statusMapMutex->lock();
                   it = statusMap.find(chID.strID);
                    if(it != statusMap.end()){
//                        statusMap.insert(std::pair<QString,int>(chID.strID,5));
                        it->second = 5;

                    }
                      statusMapMutex->unlock();
                }else if (streamStatus == -1 && recordStatus == RecordStatusStopped){
                    statusMapMutex->lock();
                  it = statusMap.find(chID.strID);
                  if(it != statusMap.end()){
//                      statusMap.insert(std::pair<QString,int>(chID.strID,6));
                      it->second = 6;

                  }
                  statusMapMutex->unlock();
                }else if(streamStatus == -1 && recordStatus == -1){
                    statusMapMutex->lock();
                   it = statusMap.find(chID.strID);
                  if(it != statusMap.end()){
//                      statusMap.insert(std::pair<QString,int>(chID.strID,7));
                      it->second = 7;

                  }
                  statusMapMutex->unlock();
                }else if(streamStatus == 0 && recordStatus == -1){
                    statusMapMutex->lock();
                   it = statusMap.find(chID.strID);
                  if(it != statusMap.end()){
//                      statusMap.insert(std::pair<QString,int>(chID.strID,8));
                      it->second = 8;

                  }
                  statusMapMutex->unlock();
                }else if(streamStatus == 1 && recordStatus == -1){
                    statusMapMutex->lock();
                   it = statusMap.find(chID.strID);
                  if(it != statusMap.end()){
//                      statusMap.insert(std::pair<QString,int>(chID.strID,9));
                      it->second = 9;

                  }
                  statusMapMutex->unlock();
                }

    }
}
/*
 * Function: getLocalPath()
 * Description:the function is to genarate a download path.
 * Calls:
 * Input: fi
 * Output:none
 * Return:none
 */
std::string Dialog::getLocalPath(FileInfoData fi){
    std::string ret="";
    ret.append(downloadPath);
    ret.append("/");
    char mpath[32];
    sprintf(mpath,"%d/",fi.mid);
    ret.append(mpath);
    char cpath[32];
    sprintf(cpath,"%d/",fi.cid);
    ret.append(cpath);
    return ret;
}
/*
 * Function: updateDownload()
 * Description:the function is update the files download status of the TableList.
 * Calls:
 * 1.getLocalPath();
 * 2.isFinished();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::updateDownload(){
//    return; //tableView have no download column.

    int i = 0;
    int totalPage = ((filteredFileInfoList.size()-1)/pageSize)+1;
    int tmp = 0,tmp1 = 0,tmp2 = 0;
    tmp = modelTableFile->rowCount() ;
    tmp1 = (filteredFileInfoList.size()) % pageSize;

    if(currentPageNo == totalPage ) {
        if(tmp1 == 0) {
            tmp2 = tmp;       
        }else {
            tmp2 = tmp1;
        }
    }else {
        tmp2 = tmp;
    }

    for(;i<tmp2;i++){
//        QStandardItem* item = modelTableFile->item(i,5);
        QStandardItem* item0 = modelTableFile->item(i,0);
        QVariant v = modelTableFile->index(i,0).data(Qt::UserRole+1);
        if(v.canConvert<FileInfoData>()){
            FileInfoData fid = v.value<FileInfoData>();

            std::string localPath = getLocalPath(fid);
            int pos = fid.fi.localUrl.find_last_of('/');
            localPath.append(fid.fi.localUrl.substr(pos));
            QFileInfo info(localPath.c_str());

            std::string displayContent="";
            if(!info.exists()){
                fid.downloadsize = 0;
            }else{
                fid.downloadsize = info.size();
            }
            if(downloadMap.find(fid.fi.downloadUrl)!=downloadMap.end()){
                fid.downloading = true;
//                qDebug("test download true\n");
            }else{
                fid.downloading = false;
//                 qDebug("test download false\n");
            }

           if(fid.downloading){
                    displayContent.append("下载中");
                    int percent = fid.downloadsize * 100 / fid.fi.size ;
                    char tmp[32];
                    sprintf(tmp,"%d%%",percent);
                    displayContent.append(tmp);
            }else{
                if(  fid.downloadsize == fid.fi.size  ){

                    displayContent.append("已下载");
                }
                else if(  fid.downloadsize == 0){

                    displayContent.append("未下载");
                }else {

                    int percent = fid.downloadsize * 100 / fid.fi.size ;
                    char tmp[32];
                    sprintf(tmp,"%d%%",percent);
                    displayContent.append(tmp);
                }
            }
            item0->setData(QVariant::fromValue(fid));
//            item->setText(QObject::tr(displayContent.c_str()));
        }

    }

    std::map<std::string,Download*>::iterator it;
    it = downloadMap.begin();
    while(it!=downloadMap.end()){
        Download* d = it->second;
        if(d->isFinished()){
//            qWarning("download is finished !\n");
            downloadMap.erase(it++);
        }else{
            it++;
        }

    }
}
/*
 * Function: on_horizontalSlider_Player_valueChanged()
 * Description:this is a slot function.when you move the slider,you will get the
 * value .
 * Calls:
 * Input: value
 * Output:none
 * Return:none
 */
/*
void Dialog::on_horizontalSlider_Player_valueChanged(int value)
{
   // qWarning("value Changed:%d\n",value);
}
*/
/*
 * Function: on_horizontalSlider_Player_sliderMoved()
 * Description:this is a slot function.when the vedio is playing,we can set
 * the slider position according to the position parameter;
 * Calls:
 * Input: position
 * Output:none
 * Return:none
 */
void Dialog::on_horizontalSlider_Player_sliderMoved(int position)
{
    ErrorInfo e;
#ifdef MYFFPLAYER
    bool isOpened = false;
#else
    int isOpened = -1 ;
#endif

    int64_t pos ;
    pos = position;
#ifdef  MYFFPLAYER
    isOpened = mPlayer->isOpened();
    if(isOpened){
        mPlayer->setPosition(pos);
    }
#else
    mcrClient->getStatusIsOpenedPlayer(isOpened,ipcUrl,e);
    if(isOpened == 1){
      mcrClient->setPosition(pos,ipcUrl,e);
    }
#endif
}
/*
 * Function: checkChannelStatus()
 * Description:this function is to get the channel parameters.according
 * to the parameters update the channel status.
 * Calls:
 *  1.asyncUpdateChannelStatus();
 * Input: ctx
 * Output:none
 * Return:none
 */
void Dialog::checkChannelStatus(void* ctx){
    CheckStatusThreadParam* param = (CheckStatusThreadParam*)ctx;
    param->dlg->asyncUpdateChannelStatus(param->channelConfig);
}

void Dialog::checkChannelStatusSingle(void* ctx){
    Dialog* param = (Dialog*)ctx;
    param->asyncCheckChannelStatusSingle();
}
void Dialog::asyncCheckChannelStatusSingle(){
    int i = 0;
    for(;i<modelTreeModule->rowCount();i++){
        QStandardItem* itemModule = modelTreeModule->item(i,0);
        if(itemModule == NULL){
            continue;
        }
        int j = 0;
        for(;j<itemModule->rowCount();j++){
            QStandardItem* itemChannel = itemModule->child(j,0);
           // asyncUpdateChannelStatus(itemChannel);
            asyncUpdateChannelStatusSingleThread(itemChannel);
        }
    }

}
void Dialog::checkChannelStatusInModule(void*ctx){
    CheckGroupStatusThreadParam* param = (CheckGroupStatusThreadParam*)ctx;
    param->dlg->asyncCheckChannelStatusInModule(param->ModuleItem);
}
void Dialog::asyncCheckChannelStatusInModule(QStandardItem* moduleItem)
{
    if(moduleItem == NULL){
        return;
    }
    int j = 0;
    for(;j<moduleItem->rowCount();j++){
        QStandardItem* itemChannel = moduleItem->child(j,0);
        asyncUpdateChannelStatusSingleThread(itemChannel);
    }
}
/*
 * Function: checkFileList()
 * Description:this function is to get the channel parameters.according
 * to the parameters to update filelist.
 * Calls:
 *  1.asyncGetChannelFileList();
 * Input: ctx
 * Output:none
 * Return:none
 */
void Dialog::checkFileList(void* ctx){
    CheckStatusThreadParam* param = (CheckStatusThreadParam*)ctx;
    param->dlg->asyncGetChannelFileList();
}
/*
 * Function: deleteFileList()
 * Description:this function is to get the parameters of pending to delete
 * fileList. And then to delete these files.
 * Calls:
 *  1.asyncDeleteChannelFileList();
 * Input: ctx
 * Output:none
 * Return:none
 */
void Dialog::deleteFileList(void* ctx){
    DeleteFileListThreadParam* param = (DeleteFileListThreadParam*)ctx;
    param->dlg->asyncDeleteChannelFileList(param->indexList);
}

void Dialog::deleteAllFile(void *ctx){
    DeleteFileListThreadParam* param = (DeleteFileListThreadParam*)ctx;
    param->dlg->asyncDeleteChannelAllFile(param->indexList);
}
/*
 * Function: deleteChannel()
 * Description:this function is to get the parameters of pending to delete
 * the channel.And then to call asyncDeleteChannel() to delete the channel.
 * Calls:
 *  1.asyncDeleteChannel();
 * Input: ctx
 * Output:none
 * Return:none
 */
void Dialog::deleteChannel(void *ctx){
    DeleteChannelThreadParam* param = (DeleteChannelThreadParam*)ctx;
    param->dlg->asyncDeleteChannel(param->indexList);
}
/*
 * Function: deleteModule()
 * Description:this function is to get the parameters of pending to delete
 * the module.And then to call asyncDeleteModule() to delete the module.
 * Calls:
 *  1.asyncDeleteModule();
 * Input: ctx
 * Output:none
 * Return:none
 */
void Dialog::deleteModule(void *ctx){
    DeleteModuleThreadParam* param = (DeleteModuleThreadParam*)ctx;
    param->dlg->asyncDeleteModule(param->indexList);
}



/*
void Dialog::on_dateEdit_Start_editingFinished()
{
    //filterFileInfoList();
    //updateFileTable();
}
*/
/*
void Dialog::on_dateEdit_Stop_editingFinished()
{
    //filterFileInfoList();
    //updateFileTable();
}
*/
/*
 * Function: SortByDate()
 * Description:this function is to compare the time stamp of f1 and f2.
 * Calls:
 * Input: f1,f2
 * Output:none
 * Return:none
 */
bool SortByDate(const FileInfo &f1, const FileInfo &f2 ){
    return f1.cTime > f2.cTime;
}
/*
 * Function: filterFileInfoList()
 * Description:this function is to filter files of tableList,and sort them according to the time.
 * Calls: none
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::filterFileInfoList(){
    QDate startDate,stopDate;
    startDate = ui->dateEdit_Start->date();
    stopDate = ui->dateEdit_Stop->date();
    filteredFileInfoList.clear();
    for(int i=0;i<fileInfoList.size();i++){
        FileInfo fi = fileInfoList[i];
        QDate currentDate = QDateTime::fromTime_t(fi.cTime).date();
        if(currentDate<=stopDate&&currentDate>=startDate){
            filteredFileInfoList.push_back(fi);
        }
    }
    std::sort(filteredFileInfoList.begin(),filteredFileInfoList.end(),SortByDate);
}
/*
 * Function: on_dateEdit_Start_dateChanged()
 * Description:this is a slot function,the function is to change the date show.
 * Calls:
 *  1. filterFileInfoList();
 *  2. updateFileTable();
 * Input: date
 * Output:none
 * Return:none
 */
void Dialog::on_dateEdit_Start_dateChanged(const QDate &date)
{
    QDate stopDate = ui->dateEdit_Stop->date();
    if(date>stopDate){
        ui->dateEdit_Start->setDate(stopDate);
    }

    currentPageNo = 1;
    filterFileInfoList();
    updateFileTable();
}
/*
 * Function: on_dateEdit_Stop_dateChanged()
 * Description:this is a slot function,the function is to change the date show.
 * Calls:
 *  1. filterFileInfoList();
 *  2. updateFileTable();
 * Input: date
 * Output:none
 * Return:none
 */
void Dialog::on_dateEdit_Stop_dateChanged(const QDate &date)
{
    QDate startDate = ui->dateEdit_Start->date();
    if(date<startDate){
        ui->dateEdit_Stop->setDate(startDate);
    }
    currentPageNo = 1;
    filterFileInfoList();
    updateFileTable();
}
/*
 * Function: keyPressEvent()
 * Description:this function is to received the press key event.when you
 * press ESC keybord it will works.And change the UI show.
 * Calls:
 *  1. showMaximized();
 * Input: e
 * Output:none
 * Return:none
 */
void Dialog::keyPressEvent(QKeyEvent *e){
    switch (e->key()) {
    case Qt::Key_Escape:
        if(fullScreen){
            ui->groupBox_Left->show();
            ui->widget_File->show();
            ui->tableView_File->show();
            ui->widget_Player->show();
            ui->widget_Title->show();
            ui->widget->show();
            //ui->verticalLayout->setMargin(0);
            ui->graphicsView_Player->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
            showMaximized();
            ui->graphicsView_Player->setGeometry(showVedioRect);
           // showVedioRect  = ui->graphicsView_Player->geometry();
            fullScreen = false;
        }
        break;
    default:
        break;
    }
}
/*
 * Function: on_pushButton_DownloadDir_clicked()
 * Description:this is a slot function.this function is to open the catalog of download file.
 * Calls:
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_DownloadDir_clicked()
{
    QModelIndexList indexList = ui->treeView_Module->selectionModel()->selectedIndexes();
    QModelIndex index;
    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<ChannelInfoData>()){
            ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
            std::string ret="";
            ret.append(downloadPath);
            ret.append("/");
            char mpath[32];
            sprintf(mpath,"%d/",channelInfoData.channelConfig.mid);
            ret.append(mpath);
            char cpath[32];
            sprintf(cpath,"%d/",channelInfoData.channelConfig.cid);
            ret.append(cpath);
            QDir dir(ret.c_str());
            qWarning("opening dir:%s\n",dir.absolutePath().toStdString().c_str());
            if(dir.exists()){
                QDesktopServices::openUrl(QUrl(dir.absolutePath().toStdString().c_str(),QUrl::TolerantMode));
            }else{
                QMessageBox::information(NULL,QObject::tr("警告"),QObject::tr("目录不存在！"),QMessageBox::Ok);
            }
        }
    }
}

// void Dialog:: updateChannelAlias(QStandardItem* itemChannel,ChannelConfig &cConf)
// {

// }
 /*
   * Function: on_pushButton_Delete_clicked()
   * Description:this is a slot function.this function is to delete file you wanted.
   * Calls:
   * 1.freshModules();
   * Input: none
   * Output:none
   * Return:none
   */
void Dialog::on_pushButton_Delete_clicked()
{
    ErrorInfo e;
#ifdef MYFFPLAYER
    bool isPlaying = false;
    isPlaying = mPlayer->isPlaying();
#else
    int isPlaying = -1;
    mcrClient->getStatusIsPlayingPlayer(isPlaying,ipcUrl,e);

#endif
    if(1 == isPlaying){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请停止播放后执行删除操作"));
        return;
    }
    QModelIndexList indexList = ui->treeView_Module->selectionModel()->selectedIndexes();
    QModelIndex index;
    foreach (index, indexList) {
       if(index.column()!=0){
             continue;
       }
    QStandardItem* item = modelTreeModule->itemFromIndex(index);
    if(item->parent() == NULL) {
        if(QMessageBox::No == QMessageBox::information(this,QObject::tr("提示"),
                                                        QObject::tr("请确认是否要删除当前模块"),
                                                        QMessageBox::Yes|QMessageBox::No,
                                                        QMessageBox::Yes)){
            return;
        }

        alertLabel->setText(QObject::tr("正在删除模块，请稍候 ...... "));
        alertDlg->resize(400,200);
        alertDlg->show();

        DeleteModuleThreadParam* param = new DeleteModuleThreadParam;
        param->dlg = this;
        param->indexList = indexList;
        deleteModuleThread = new WorkerThread(param,deleteModule);
        deleteModuleThread->setStackSize(1*1024*1024);
        deleteModuleThread->start();
        while(deleteModuleThread->isRunning()){
            QApplication::processEvents();
        }
        delete deleteModuleThread;
        delete param;

        alertDlg->hide();
        freshModules();
    }else {
        if(QMessageBox::No == QMessageBox::information(this,QObject::tr("提示"),
                                                        QObject::tr("请确认是否要删除当前通道"),
                                                        QMessageBox::Yes|QMessageBox::No,
                                                        QMessageBox::Yes)){
            return;
        }

        alertLabel->setText(QObject::tr("正在删除通道，请稍候 ...... "));
        alertDlg->resize(400,200);
        alertDlg->show();

        DeleteChannelThreadParam* param = new DeleteChannelThreadParam;
        param->dlg = this;
        param->indexList = indexList;
        deleteChannelThread = new WorkerThread(param,deleteChannel);
        deleteChannelThread->setStackSize(1*1024*1024);
        deleteChannelThread->start();
        while(deleteChannelThread->isRunning()){
            QApplication::processEvents();
        }
        delete deleteChannelThread;
        delete param;

        alertDlg->hide();
          freshModules();
    }
    }
    return;
}
/*
 * Function: on_pushButton_Add_clicked()
 * Description:this is a slot function.this function is to add channel/module.
 * Calls:
 * 1.setChannelConfig();
 * 2.freshModules();
 * 3.setModuleConfig();
 * Input: none
 * Output:none
 * Return:none
 */
void Dialog::on_pushButton_Add_clicked()
{

    QModelIndexList indexList = ui->treeView_Module->selectionModel()->selectedIndexes();
    QModelIndex index;
    foreach (index, indexList) {
       if(index.column()!=0){
             continue;
       }
    QStandardItem* item = modelTreeModule->itemFromIndex(index);
    if(item->parent() == NULL) {

        QStringList Options;
        Options << tr("通道")<< tr("模块");
        bool ok;
        QVariant v;
        v = item->data(Qt::UserRole + 1);
        if(v.canConvert<ModuleInfoData>()) {

        QString OptionItem = QInputDialog::getItem(this,tr("选择对话框"),tr("请选择增加项："),Options,0,false,&ok);

        if(ok && !Options.isEmpty()){
        if(OptionItem.compare(tr("通道")) ==0) {
            ModuleInfoData moduleInfoData;
            ChannelInfoData channelInfoData;
            QVariant v = item->data(Qt::UserRole +1);
            if(v.canConvert<ModuleInfoData>()){
                moduleInfoData = v.value<ModuleInfoData>();
                channelInfoData.channelConfig.mid = moduleInfoData.moduleConfig.mid;
            }
            //get module_*.ini ip info

            std::string tmp_serverUrl = "";
            QString aDir = QCoreApplication::applicationDirPath();
            aDir += "/module_"+QString::number(channelInfoData.channelConfig.mid)+".ini";
            qWarning(aDir.toStdString().c_str());

            config = new QSettings(aDir,QSettings::IniFormat);
            tmp_serverUrl = config->value("/network/ip").toString().toStdString();

            AddChannelDialog addChannelDialog;
            addChannelDialog.setChannelConfig(tmp_serverUrl,channelInfoData,&addChannelDialog);
            addChannelDialog.exec();
            if(addChannelDialog.flag == 1) {
                addChannelDialog.flag = 0;
                freshModules();
            }
        }else if(OptionItem.compare(tr("模块"))==0){

            AddModuleDialog addModuleDialog;
            addModuleDialog.setModuleConfig(&addModuleDialog);
            addModuleDialog.exec();
            if(addModuleDialog.flag == 1){
                addModuleDialog.flag = 0;
                freshModules();
            }
        }
       }
      }else{
            AddModuleDialog addModuleDialog;
            addModuleDialog.setModuleConfig(&addModuleDialog);
            addModuleDialog.exec();
            if(addModuleDialog.flag == 1){
                addModuleDialog.flag = 0;
                freshModules();
            }
      }
    }else {
        ChannelInfoData channelInfoData;
        QVariant v = item->data(Qt::UserRole +1);
        if(v.canConvert<ChannelInfoData>()){
            channelInfoData = v.value<ChannelInfoData>();
        }
        //get module_*.ini ip info
        QString aDir = QCoreApplication::applicationDirPath();
        std::string tmp_serverUrl;

        aDir += "/module_"+QString::number(channelInfoData.channelConfig.mid)+".ini";
        qWarning(aDir.toStdString().c_str());

        config = new QSettings(aDir,QSettings::IniFormat);
        tmp_serverUrl = config->value("/network/ip").toString().toStdString();

        AddChannelDialog addChannelDialog;
        addChannelDialog.setChannelConfig(tmp_serverUrl,channelInfoData,&addChannelDialog);
        addChannelDialog.exec();
        if(addChannelDialog.flag == 1) {
            addChannelDialog.flag = 0;
            freshModules();
        }
        }

    }
    return;
}

void Dialog::freshChannels(ChannelInfoData &channelInfoData)
{
//    ErrorInfo e;
//    ModuleInfoData moduleInfoData;

//    modelTreeModule = new QStandardItemModel(1,3,ui->treeView_Module);
//    ui->treeView_Module->setModel(modelTreeModule);
//    ui->treeView_Module->setAlternatingRowColors(true);
//    modelTreeModule->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("状态")));
//    modelTreeModule->setHorizontalHeaderItem(1,new QStandardItem(QObject::tr("信息")));
//    modelTreeModule->setHorizontalHeaderItem(2,new QStandardItem(QObject::tr("设置")));
//    ui->treeView_Module->header()->setResizeMode(1,QHeaderView::Stretch);
//    ui->treeView_Module->setColumnWidth(0,120);
//    ui->treeView_Module->setColumnWidth(2,60);
//    ui->treeView_Module->setHeaderHidden(true);
//    ui->treeView_Module->setSelectionMode(QAbstractItemView::SingleSelection);
//    ui->treeView_Module->setIconSize(QSize(40,80));

//    mcrClient->getModuleConfig(moduleInfoData.moduleConfig,channelInfoData.channelConfig.mid,moduleServerUrl.c_str(),e);

//    QStandardItem* itemModule = new QStandardItem;
//    itemModule->setData(QVariant::fromValue(moduleInfoData));
//    updateChannels(itemModule);
}

void Dialog::on_pushButton_DeleteAllFile_clicked()
{
    ErrorInfo e;

#ifdef MYFFPLAYER
    bool isPlaying = false;
    isPlaying = mPlayer->isPlaying();
#else
    int isPlaying = -1;
    mcrClient->getStatusIsPlayingPlayer(isPlaying,ipcUrl,e);

#endif
    if(1 == isPlaying){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请停止播放后执行删除操作"));
        return;
    }
    //------------------------
    QModelIndexList indexList2 = ui->treeView_Module->selectionModel()->selectedIndexes();
    QModelIndex index2;

    foreach(index2,indexList2){
        if(index2.column()!=0){
            continue;
        }
        QVariant v2 = index2.data(Qt::UserRole+1);

        if(v2.canConvert<ChannelInfoData>()){
            ErrorInfo e;

            ChannelInfoData cInfoData = v2.value<ChannelInfoData>();
            if(cInfoData.streamOnline == 0){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道未在线"));
                return ;
            }else if(cInfoData.recordOnline == 0){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("文件服务器未在线"));
                return;
            }
        }
    }

    //--------------------------

    //判断该通道是否有文件正在播放或下载

    //##################播放

    QModelIndexList indexList_1 = ui->treeView_Module->selectionModel()->selectedIndexes();
    QModelIndex index_1;
    foreach (index_1, indexList_1) {
        if(index_1.column()!=0){
            continue;
        }
        QVariant v = index_1.data(Qt::UserRole+1);
        if(v.canConvert<ChannelInfoData>()){
            ErrorInfo e;

            ChannelInfoData cInfoData = v.value<ChannelInfoData>();

            if(playing_mid == cInfoData.channelConfig.mid && playing_cid == cInfoData.channelConfig.cid){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道中有文件正在播放,不能删除该通道所有文件！"));
                return;
            }

        }
    }
    //-----------------------------
    //judge  delete the channel all file.
    //------------------------------

    //###############下载

    std::vector<FileInfo>::iterator it;
    for(it=filteredFileInfoList.begin();it!=filteredFileInfoList.end();it++){

            FileInfoData fid ;
            fid.downloading = false;
            if(downloadMap.find(it->downloadUrl)!=downloadMap.end()){
                fid.downloading = true;
                qDebug("test download true\n");
            }else{
                fid.downloading = false;
                qDebug("test download false\n");
            }      
           if(fid.downloading){
              QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道中有文件正在下载,不能删除该通道所有文件！"));
              return;
           }



    }

    //###############

    QModelIndexList indexList = ui->treeView_Module->selectionModel()->selectedIndexes();
    QModelIndex index;
//-------------------------------
//判断是否正在删除全部文件
    foreach (index, indexList) {
        if(index.column()!=0){
            continue;
        }
        QVariant v = index.data(Qt::UserRole+1);
        if(v.canConvert<ChannelInfoData>()){
            ErrorInfo e;
            int recordStatus;
            ChannelInfoData cInfoData = v.value<ChannelInfoData>();  

            mcrClient->getChannelRecordStatus(recordStatus,cInfoData.channelConfig.mid,cInfoData.channelConfig.cid,cInfoData.channelConfig.recordServerUrl.c_str(),e);
            if(e.result==-256){
                cInfoData.status = 0;
            }

            if(recordStatus == 1){
               QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道正在录制中"));
               return;
            }
            qDebug("channel isDelete %d",cInfoData_isDelete.isDelete);
            if(cInfoData.isDelete == 1){
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("该通道正在删除文件"));
                return;
            }
        }
    }


    if(QMessageBox::No == QMessageBox::information(this,QObject::tr("提示"),
                                                    QObject::tr("请确认是否要删除该通道全部文件"),
                                                    QMessageBox::Yes|QMessageBox::No,
                                                    QMessageBox::Yes)){
        return;
    }


   // alertLabel->setText(QObject::tr("正在删除该通道全部文件，请稍候 ...... "));
   // alertDlg->resize(400,200);
   // alertDlg->show();

    DeleteFileListThreadParam* param = new DeleteFileListThreadParam;
    param->dlg = this;
    param->indexList = indexList;
    deleteAllFileThread = new WorkerThread(param,deleteAllFile);
    deleteAllFileThread->setStackSize(1*1024*1024);
    deleteAllFileThread->start();
    while(deleteAllFileThread->isRunning()){
        QApplication::processEvents();
    }
    delete deleteAllFileThread;
    delete param;



}
void Dialog::finish_process(int exitCode, QProcess::ExitStatus exitStatus)
{

    if (exitStatus == QProcess::NormalExit)
    {
         qDebug("pro finish normal");
    }
    else
    {
        qDebug("pro finish  not normal");
    }

}
void Dialog::updateProStatus()
{
    //getModules();
    int i = 0;
    for(;i<modelTreeModule->rowCount();i++){
        QStandardItem* itemModule = modelTreeModule->item(i,0);
        if(itemModule == NULL){
            continue;
        }
        int j = 0;
        for(;j<itemModule->rowCount();j++){
            QStandardItem* itemChannel = itemModule->child(j,0);
            asyncUpdateProStatus(itemChannel);
        }
    }
   // qDebug("update process status");
}
void Dialog::start_process()
{
    qDebug("pro is starting");
    cInfoData_isDelete.isDelete = 1;
}
void Dialog::asyncUpdateProStatus(QStandardItem* itemChannel)
{
    QVariant v = itemChannel->data();
    if(v.canConvert<ChannelInfoData>()){
        ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
        char fName[10] = {0};
        sprintf(fName,"%d_%d.txt",channelInfoData.channelConfig.mid,channelInfoData.channelConfig.cid);

        int flash_channel = 0;
        if(channelInfoData.isDelete == 1){
            flash_channel = 1;
        }
        QFile file(fName);
        if(!file.exists()){
           // qDebug("file  %s no exist\n",fName);

            if(flash_channel == 1){
                //删除通道中的所有文件,并重新获取文件服务端的文件
                //###############
                if(currentChannelConfig){
                    delete currentChannelConfig;
                }
                currentChannelConfig = new ChannelConfig;
                *currentChannelConfig = channelInfoData.channelConfig;
                //clear the file table list
                fileInfoList.clear();

                CheckFileListThreadParam* param = new CheckFileListThreadParam;
                param->dlg = this;

                checkFileListThread = new WorkerThread(param,checkFileList);
                checkFileListThread->setStackSize(1*1024*1024);
                checkFileListThread->start();
                while(checkFileListThread->isRunning()){
                    QApplication::processEvents();
                }
                delete checkFileListThread;
                checkFileListThread = NULL;
                delete param;

                currentPageNo = 1;
                ui->dateEdit_Start->setDate(QDate::fromString("2000-01-01"));
                ui->dateEdit_Stop->setDate(QDate::currentDate());
                filterFileInfoList();
                updateFileTable();

//                int j;
//                for(j=0;j<pageSize;j++){
//                      QStandardItem* item5 = modelTableFile->item(j,5);;
//                      item5->setText("");
//                }

                //###############
                channelInfoData.isDelete = 0;
                flash_channel = 0;
            }
        }else{
            qDebug("file Name is %s\n",fName);
        }
        //关闭qprocess 进程
        if(channelInfoData.pro != NULL){
            if(QProcess::NormalExit == channelInfoData.pro->exitStatus()){
                qDebug("pro has closed");
                channelInfoData.pro = NULL;
                delete  channelInfoData.pro ;
            }

        }
        itemChannel->setData(QVariant::fromValue(channelInfoData));
    }
}



void Dialog::updatePosition()
{

}
bool Dialog::eventFilter(QObject *target, QEvent *event)
{

if (ui->horizontalSlider_Player == target)
{

    QMouseEvent *mev = static_cast<QMouseEvent*>(event);
    if (QEvent::MouseButtonPress == event->type() && ui->horizontalSlider_Player->isEnabled())
    {
     //   int size = m_stPara.nWidth * m_stPara.nHeight;
        int size = 1000;
//根据比例计算新进度条的位置
       // int pos = m_pManager->GetFrameNum(size) * mev->x() / width();
        int pos =  size * mev->x() /ui->horizontalSlider_Player->width();
        //qWarning("mouse event x=%d,width=%d",mev->x(),);
        ui->horizontalSlider_Player->setValue(pos);
       //SilderChange(pos);
        on_horizontalSlider_Player_sliderMoved(pos);
   }else if (QEvent::MouseMove == event->type() && ui->horizontalSlider_Player->isEnabled()){
          //  int size = m_stPara.nWidth * m_stPara.nHeight;
            int size =  1000;
         //   int pos = m_pManager->GetFrameNum(size) * mev->x() / width();
           int pos = size * mev->x() /ui->horizontalSlider_Player->width();
            ui->horizontalSlider_Player->setValue(pos);
         //   SilderChange(pos);
            on_horizontalSlider_Player_sliderMoved(pos);
  }
        return QWidget::eventFilter(target, event);
    }
    return false;
}

void Dialog::deleteMid(int delMid)
{
    QSettings *mCount;
    std::string mid_count;
    std::string insert_mid = "";
    QVector<int> tmp_mid;
    QVector<int>::iterator it;


    QString tmp = "";
    const char* pmid = NULL;
    QString mDir = QCoreApplication::applicationDirPath();
    mDir += "/module_count.ini";
    qWarning(mDir.toStdString().c_str());
    mCount = new QSettings(mDir,QSettings::IniFormat);
    mid_count = mCount->value("/count/count").toString().toStdString();
    pmid = mid_count.c_str();
    //get mid from module_count.ini file
    for(int i=0;i<mid_count.length();i++){
        if(pmid[i] != ','){
            tmp += pmid[i];
            if(mid_count.length() - 1 == i){
                tmp_mid.push_back(tmp.toInt());
            }
        }else{
            tmp_mid.push_back(tmp.toInt());
            tmp = "";
        }
    }
    qSort(tmp_mid.begin(),tmp_mid.end());

    //------------------------------
    //delete the delMid from file of module_count.ini

    it = tmp_mid.begin();

    for(int i=0;it!=tmp_mid.end();i++){
        if(*it == delMid){
            if(it == tmp_mid.end()){
                tmp_mid.erase(it);
            }else{
             tmp_mid.erase(it++);
            }
           break;
        }else{
            it++;
        }
    }


    qSort(tmp_mid.begin(),tmp_mid.end());
//---------------------------------------------
    it = tmp_mid.begin();
    for(int i=0;i<tmp_mid.count();i++,it++){

        if(it+1 != tmp_mid.end()){
            insert_mid.append(QString::number(*it).toStdString()+',');
        }else{
            insert_mid.append(QString::number(*it).toStdString());
        }
    }


    mCount->setValue("/count/count",QString::fromStdString(insert_mid));

    return;
}



void Dialog::slotHelp()
{
    Info *info = new Info();
    info->exec();
}
void Dialog::slotShowSmall()
{
    this->showMinimized();
}

void Dialog::slotShowMaxRestore()
{
    if (isMax_) {
        this->showNormal();
        ui->pushButton_max->setIcon(maxPixmap_);
    } else {
        this->showMaximized();
        ui->pushButton_max->setIcon(restorePixmap_);
    }
    isMax_ = !isMax_;
}

void Dialog::on_pushButton_update_clicked()
{
    UpdateDialog *uDialog = new UpdateDialog();
    uDialog->exec();
}



void Dialog::on_pushButton_PlayerPause_clicked()
{

//    if(player->isOpened() && player->isPlaying()){
//        if(player->isLive()){
//            return ;
//        }
//        player->pause();
//    }
}


void Dialog::showVedio(void * ctx){
    Dialog* dlg = (Dialog*)ctx;
    dlg->asyncShowVedio();
}

void Dialog::asyncShowVedio()
{
    ErrorInfo e;
    e.result = 0;
#ifdef  MYFFPLAYER
    bool isPlaying = false;
    isPlaying = mPlayer->isPlaying();
#else
    int isPlaying = 1;
    mcrClient->getStatusIsPlayingPlayer(isPlaying,ipcUrl,e);
    if(e.result == -256){
        mcrClient->getStatusIsPlayingPlayer(isPlaying,ipcUrl,e);
    }
#endif
    if(isPlaying)
    {
        QPixmap pix ;

        semTake(semIdShare, WAIT_FOREVER);


        memcpy(imageVideo->bits(),pData+64,WIDTH*HIGH*4);
        semGive(semIdShare);

        {/*
            uchar *p = imageVideo->bits();
        for(int i=0;i<50;i++){
            for(int j=0;j<100;j++){
                p[i*WIDTH*4+j*4] = 255;
            }
        }
        */
        }
//        QImage image(imageVideo->bits(),showVedioRect.width(),showVedioRect.height(),QImage::Format_RGB32);
//        if(image.save("test.jpg","JPEG")){
//            qDebug("image save successful.");
//        }else{
//            qDebug("image save error.");
//        }
        pix = QPixmap::fromImage(*imageVideo);
//       qDebug("--------------1.");
        if(false == fullScreen){
            ui->graphicsView_Player->setPixmap(pix.scaled(showVedioRect.width() - VEDIOWIDTH,showVedioRect.height() - VEDIOHEIGHT));
        }else{
           ui->graphicsView_Player->setPixmap(pix.scaled(SCREENWIDTH - fVEDIOWIDTH,SCREENHEIGHT - fVEDIOHEIGHT));
           /*
            ui->graphicsView_Player->setPixmap(pix.scaled(ui->graphicsView_Player->width() - fVEDIOWIDTH,
                                                         ui->graphicsView_Player->height() - fVEDIOHEIGHT));
           */
//         qDebug()<< ui->graphicsView_Player->geometry();
        }
    }else{
       ui->graphicsView_Player->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    }
}


void Dialog::traverseTree()
{
    int i = 0;
       for(;i<modelTreeModule->rowCount();i++){
           QStandardItem* itemModule = modelTreeModule->item(i,0);
           if(itemModule == NULL){
               continue;
           }

           for(int j = 0;j<itemModule->rowCount();j++){
               QStandardItem* itemChannel = itemModule->child(j,0);
               traverseChannelsUpdateStatus(itemChannel);
           }
       }
}
void Dialog::traverseChannelsUpdateStatus(QStandardItem*itemChannel)
{

      QVariant v = itemChannel->data();

      if(v.canConvert<ChannelInfoData>()){

        ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
        channelID chID;

        chID.mid = channelInfoData.channelConfig.mid;
        chID.cid = channelInfoData.channelConfig.cid;
        channelInfoData.streamStatus = 0;
        channelInfoData.streamOnline = 0;
        channelInfoData.recordStatus = 0;
        channelInfoData.recordOnline = 0;
       // qWarning("mid is %d,cid is %d\n",chID.mid,chID.cid);
//        chID.strID = QString("%1_%2").arg(chID.mid,chID.cid);
        chID.strID = QString("%1").arg(chID.mid)+"_"+QString::number(chID.cid);


        std::map<QString,int>::iterator it;
        statusMapMutex->lock();
        it = statusMap.find(chID.strID) ;
        if(it != statusMap.end()){
            int statusValue = it->second;
            statusMapMutex->unlock();

                    if(statusValue == 1){
                        channelInfoData.streamStatus = 1;
                        channelInfoData.recordStatus = 1;
                        channelInfoData.streamOnline = 1;
                        channelInfoData.recordOnline = 1;
                        itemChannel->setIcon(QIcon(":/image/playing&recording.png"));
                        itemChannel->setToolTip(QObject::tr("有信号源\r\n正在录制"));
                      //  qWarning("%d_%d:1--1  %s\n",chID.mid,chID.cid,chID.strID.toStdString().c_str());
                    }else if(statusValue == 2){
                        channelInfoData.streamStatus = 1;
                        channelInfoData.recordStatus = 0;
                        channelInfoData.streamOnline = 1;
                        channelInfoData.recordOnline = 1;
                        itemChannel->setIcon(QIcon(":/image/playing&norecording.png"));
                         itemChannel->setToolTip(QObject::tr("有信号源\r\n未录制"));
                       // qWarning("%d_%d:1--0  %s\n",chID.mid,chID.cid,chID.strID.toStdString().c_str());
                    }else if(statusValue == 3){
                        channelInfoData.streamStatus = 0;
                        channelInfoData.recordStatus = 0;
                        channelInfoData.streamOnline = 1;
                        channelInfoData.recordOnline = 1;
                        itemChannel->setIcon(QIcon(":/image/stop&norecording.png"));
                        itemChannel->setToolTip(QObject::tr("无信号源\r\n未录制"));
                       // qWarning("%d_%d:0--0  %s\n",chID.mid,chID.cid,chID.strID.toStdString().c_str());
                    }else if(statusValue == 4){
                        channelInfoData.streamStatus = 0;
                        channelInfoData.recordStatus = 1;
                        channelInfoData.streamOnline = 1;
                        channelInfoData.recordOnline = 1;
                        itemChannel->setIcon(QIcon(":/image/stop&recording.png"));
                        itemChannel->setToolTip(QObject::tr("无信号源\r\n正在录制"));
                       // qWarning("%d_%d:0--1  %s\n",chID.mid,chID.cid,chID.strID.toStdString().c_str());
                    }else if(statusValue == 5){
                        channelInfoData.streamOnline = 0;
                        channelInfoData.streamStatus = 0;
                        channelInfoData.recordOnline = 1;
                      //  channelInfoData.recordStatus = 0;
                        itemChannel->setIcon(QIcon(":/image/stop&norecording.png"));
                        itemChannel->setToolTip(QObject::tr("无信号源\r\n未录制"));
                    }else if(statusValue == 6){
                        channelInfoData.streamOnline = 0;
                        channelInfoData.streamStatus = 0;
                        channelInfoData.recordOnline = 1;
                       // channelInfoData.recordStatus = 0;

                    }else if(statusValue == 7){
                        channelInfoData.streamOnline = 0;
                        channelInfoData.streamStatus = 0;
                        channelInfoData.recordOnline = 0;
                       // channelInfoData.recordStatus = 0;

                    }else if(statusValue == 8){
                        channelInfoData.streamOnline = 1;
                        channelInfoData.streamStatus = 0;
                        channelInfoData.recordOnline = 0;
                       // channelInfoData.recordStatus = 0;

                    }else if(statusValue == 9){
                        channelInfoData.streamOnline = 1;
                        channelInfoData.streamStatus = 1;
                        channelInfoData.recordOnline = 0;
                       // channelInfoData.recordStatus = 0;

                    }
                   itemChannel->setData(QVariant::fromValue(channelInfoData));
                   itemChannel->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

        }else{
            statusMapMutex->unlock();
        }
      }
}

void Dialog::on_pBt_shift_clicked()
{
    ShiftLogin * shiftLogin = ShiftLogin::getInstance();
    shiftLogin->clearPassword();
    shiftLogin->exec();
    int result = shiftLogin->getResult();
    if(result != 1){
        return;
    }

//    QMessageBox box;
//    box.setWindowTitle(QObject::tr("提示"));
//    box.setIcon(QMessageBox::Information);
//    box.setStandardButtons(QMessageBox::Yes|QMessageBox::Ok|QMessageBox::Open);
//    box.setButtonText(QMessageBox::Yes,tr("HKB"));
//    box.setButtonText(QMessageBox::Ok,tr("BD"));
//    box.setButtonText(QMessageBox::Open,tr("屏幕录制"));

    ShiftMessageBox * shiftMessageBox = ShiftMessageBox::getInstance();
    shiftMessageBox->resetResult();
    shiftMessageBox->exec();
    int res = shiftMessageBox->getResult();

    if(res == 1){
        QFileInfo fInfo("/ahci00:1/menu.lst");
        if(!fInfo.exists()){
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("/ahci00:1/路径下不存在menu.lst！"),QMessageBox::Ok);
            return;
        }

        QFileInfo fInfo_seri("/ahci00:1/menu.lst.seri");
        if(!fInfo_seri.exists()){
            QMessageBox::information(this,QObject::tr("提示"),QObject::tr("/ahci00:1/路径下不存在menu.lst.seri！"),QMessageBox::Ok);
            return;
        }
        QFileInfo fInfo_BD("/ahci00:1/HKB.sh");
        if(!fInfo_BD.exists()){
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("/ahci00:1/路径下不存在HKB.sh！"),QMessageBox::Ok);
            return;
        }

        int ret = rename("/ahci00:1/menu.lst","/ahci00:1/menu.lst.topmoo");
        if(ret == 0){
            struct timespec tv;
            struct timespec tv_rval;
            tv.tv_sec = 0;
            tv.tv_nsec = 100000;
            nanosleep(&tv,&tv_rval);
        }else {
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("重命名menu.lst to menu.lst.topmoo失败！"),QMessageBox::Ok);
            return;
        }
        ret = rename("/ahci00:1/menu.lst.seri","/ahci00:1/menu.lst");

        if(ret == 0){
            struct timespec tv;
            struct timespec tv_rval;
            tv.tv_sec = 0;
            tv.tv_nsec = 100000;
            nanosleep(&tv,&tv_rval);
        }else{
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("重命名menu.lst.seri to menu.lst失败！"),QMessageBox::Ok);
            return;
        }

        ret = rename("/ahci00:1/HKB.sh","/ahci00:1/start.sh");
        if(ret == 0){
            struct timespec tv;
            struct timespec tv_rval;
            tv.tv_sec = 0;
            tv.tv_nsec = 100000;
            nanosleep(&tv,&tv_rval);
            ::sysReboot();
        }else{
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("重命名HKB.sh to start.sh失败！"),QMessageBox::Ok);
            return;
        }
    }else if(res == 2){
        QFileInfo fInfo("/ahci00:1/menu.lst");
        if(!fInfo.exists()){
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("/ahci00:1/路径下不存在menu.lst！"),QMessageBox::Ok);
            return;
        }

        QFileInfo fInfo_seri("/ahci00:1/menu.lst.seri");
        if(!fInfo_seri.exists()){
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("/ahci00:1/路径下不存在menu.lst.seri！"),QMessageBox::Ok);
            return;
        }
        QFileInfo fInfo_BD("/ahci00:1/BD.sh");
        if(!fInfo_BD.exists()){
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("/ahci00:1/路径下不存在BD.sh！"),QMessageBox::Ok);
            return;
        }

        int ret = rename("/ahci00:1/menu.lst","/ahci00:1/menu.lst.topmoo");
        if(ret == 0){
            struct timespec tv;
            struct timespec tv_rval;
            tv.tv_sec = 0;
            tv.tv_nsec = 100000;
            nanosleep(&tv,&tv_rval);
        }else {
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("重命名menu.lst to menu.lst.topmoo失败！"),QMessageBox::Ok);
            return;
        }
        ret = rename("/ahci00:1/menu.lst.seri","/ahci00:1/menu.lst");

        if(ret == 0){
            struct timespec tv;
            struct timespec tv_rval;
            tv.tv_sec = 0;
            tv.tv_nsec = 100000;
            nanosleep(&tv,&tv_rval);
        }else{
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("重命名menu.lst.seri to menu.lst失败！"),QMessageBox::Ok);
            return;
        }

        ret = rename("/ahci00:1/BD.sh","/ahci00:1/start.sh");
        if(ret == 0){
            struct timespec tv;
            struct timespec tv_rval;
            tv.tv_sec = 0;
            tv.tv_nsec = 100000;
            nanosleep(&tv,&tv_rval);
            ::sysReboot();
        }else{
            QMessageBox::information(NULL,QObject::tr("提示"),QObject::tr("重命名BD.sh to start.sh失败！"),QMessageBox::Ok);
            return;
        }
    }else if(res == 3){
        return;
    }
}
