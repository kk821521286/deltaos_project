#include "channeldialog.h"
#include "ui_channeldialog.h"


#include"mcrclient.h"
#include<QMessageBox>
#include"combodelegate.h"
#include"moduleitemdelegate.h"
#include<string.h>



ChannelDialog::ChannelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChannelDialog)
{

    ui->setupUi(this);

    Qt::WindowFlags flags;

    flags = this->windowFlags();
    flags = flags &(~Qt::WindowTitleHint);
    flags = flags &(~Qt::WindowSystemMenuHint);
    flags = flags &(~Qt::WindowContextHelpButtonHint);
    this->setWindowFlags(flags);
    alertDlg = new QDialog(this);



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
    //========
        QString aDir = QCoreApplication::applicationDirPath();
        aDir += "/config.ini";
        qWarning(aDir.toStdString().c_str());
        config = new QSettings(aDir,QSettings::IniFormat);
    //=========
   // config = new QSettings("config.ini",QSettings::IniFormat);
    moduleServerUrl = config->value("/system/server").toString().toStdString();
    if(moduleServerUrl==""){
        moduleServerUrl = "http://127.0.0.1/soapserver.php";
    }





}

ChannelDialog::~ChannelDialog()
{   
    delete ui;
}
/*
 * Function: showChannelConfig()
 * Description:this function is to show channel config information on
 *  config interface.
 * Calls:
 * 1.getChannelConfig();
 * Input: none
 * Output:none
 * Return:none
 */
void ChannelDialog::showChannelConfig()
{
   QStandardItem* item;
   int Mid,Cid;
    int i;
   ErrorInfo e;

   Mid = v_cConf.mid;
   Cid = v_cConf.cid;
   //Mid = 0;
  // Cid = 1;
   qDebug("------------------%d",Mid);
   qDebug("------------------%d",Cid);
   //获取当前模块的IP
   QString aDir = QCoreApplication::applicationDirPath();
   aDir += "/module_"+ QString::number(Mid) +".ini";
   qWarning(aDir.toStdString().c_str());
   config = new QSettings(aDir,QSettings::IniFormat);

    std::string moduleServerUrl;

   moduleServerUrl = "http://";
   moduleServerUrl += config->value("/network/ip").toString().toStdString();
   moduleServerUrl +=  "/soapserver.php";
    qDebug("befor get channel config\n");
   mcrClient->getChannelConfig(cConf, Mid, Cid, moduleServerUrl.c_str(), e);
    qDebug("after get channel config\n");
   if(e.result != 0) {
       qWarning("get channel config error\n");
   }
    QString qstr;
    qstr = "通道";
   qWarning("get channel config successful\n");
   for(i=0;i<10;i++) {
    item  = modelTablecConf->item(i,0);
    if(i == 0) //mid
    item->setText(QString::number(cConf.mid));
    if(i == 1) //cid
    item->setText(QString::number(cConf.cid));
    if(i == 2) //name
    item->setText(QObject::tr(cConf.name.c_str()));
    if(i == 3) //alias
    item->setText(QObject::tr(cConf.alias.c_str()));
    item->setEnabled(false);
    if(i == 4) {//ip
        item->setText(cConf.ip.c_str());
        item->setEnabled(false);
    }
    if(i == 5) {//type
        if(cConf.type == 1) {
            item->setText(tr("VGA"));
            item->setEnabled(false);
        }else if (cConf.type == 2) {
            item->setText(tr("AV"));
            item->setEnabled(false);
        }else if (cConf.type == 3) {
            item->setText(tr("Audio"));
            item->setEnabled(false);
        }
    }
    if(i == 6) {//streamUrl
        item->setText(cConf.streamUrl.c_str());
        item->setEnabled(false);
    }
    if(i == 7) {//streamServerUrl
        item->setText(cConf.streamServerUrl.c_str());
        item->setEnabled(false);
    }
    if(i == 8) {//recordServerUrl
        item->setText(cConf.recordServerUrl.c_str());
        item->setEnabled(false);
    }
    if(i == 9) {//fileServerUrl
    item->setText(cConf.fileServerUrl.c_str());
    item->setEnabled(false);
    }
   }

//========================================================



}
/*
 * Function: showStreamConfig()
 * Description:this function is to show stream config information on
 *  config interface.
 * Calls:
 * 1.getChannelStreamConfig();
 * Input: none
 * Output:none
 * Return:none
 */
void ChannelDialog::showStreamConfig()
{


    QStandardItem* item;

    int Mid,Cid;
    int i;
    ErrorInfo e;


    Mid = v_cConf.mid;
    Cid = v_cConf.cid;

   // Mid = 0;
   // Cid = 1;

    qWarning("befor get stream config \n");
    qDebug("stream url is %s\n",cConf.streamServerUrl.c_str());
    if(cConf.streamServerUrl.c_str() != "")
     mcrClient->getChannelStreamConfig(sConf, Mid, Cid, cConf.streamServerUrl.c_str(), e);
    qWarning("after get stream config \n");
    if(e.result != 0) {
        qWarning("get stream config error\n");
    }

    qWarning("get stream config successful\n");
    for(i=0;i<8;i++) {
     item  = modelTablesConf->item(i,0);
     if(i == 0) //mid
     item->setText(QString::number(sConf.mid));
     if(i == 1) //cid
     item->setText(QString::number(sConf.cid));
     if(i == 2) //media_codec1_video_width
     item->setText(QString::number(sConf.media_codec1_video_width));
     //item->setEnabled(false);
     if(i == 3) //media_codec1_video_height
     item->setText(QString::number(sConf.media_codec1_video_height ));
     //item->setEnabled(false);
     if(i == 4) //media_codec1_video_bitrate
     item->setText(QString::number(sConf.media_codec1_video_bitrate ));
    // item->setEnabled(false);
     if(i == 5) //media_codec1_video_maxbitrate
     item->setText(QString::number(sConf.media_codec1_video_maxbitrate ));
    // item->setEnabled(false);
     if(i == 6) //media_capture_audio_samplerate
     item->setText(QString::number(sConf.media_capture_audio_samplerate));
     //item->setEnabled(false);
     if(i == 7) //media_codec1_audio_bitrate
     item->setText(QString::number(sConf.media_codec1_audio_bitrate) );
     //item->setEnabled(false);

    }



}
/*
 * Function: showRecordConfig()
 * Description:this function is to show record config information on
 *  config interface.
 * Calls:
 * 1.getChannelRecordConfig();
 * Input: none
 * Output:none
 * Return:none
 */
void ChannelDialog::showRecordConfig()
{

    QStandardItem* item;

    int Mid,Cid;
    int i;
    ErrorInfo e;


    Mid = v_cConf.mid;
    Cid = v_cConf.cid;

   // Mid = 0;
   // Cid = 1;

    qDebug("befor get record config\n");
    qDebug("record url is %s\n",cConf.recordServerUrl.c_str());
    if(cConf.recordServerUrl.c_str() != "")
    mcrClient->getChannelRecordConfig(rConf, Mid, Cid, cConf.recordServerUrl.c_str(), e);
    qDebug("after get channel config\n");
   if(e.result != 0) {
       qWarning("get record config error\n");
   }

   qWarning("get record config successful\n");
   for(i=0;i<7;i++) {
    item  = modelTablerConf->item(i,0);
    if(i == 0) //mid
    item->setText(QString::number(rConf.mid));
    if(i == 1) //cid
    item->setText(QString::number(rConf.cid));
    if(i == 2) {//srcUrl
        item->setText(rConf.srcUrl.c_str());
        item->setEnabled(false);
    }
    if(i == 3) //prefix
    item->setText(rConf.prefix.c_str());
    if(i == 4) {//type
        item->setText(rConf.type.c_str());
        item->setEnabled(false);
    }
    if(i == 5) //segment
    item->setText(QString::number(rConf.segment));
    if(i == 6){//timeServer
        item->setText(rConf.timeServer.c_str());
    }
   }

qDebug("timeServer :%s",rConf.timeServer.c_str());

}
/*
 * Function: setChannelConfig()
 * Description:this function is to get channel config information from
 *  treeView.
 * Calls:
 * 1.showChannelInterface();
   2.showStreamInterface();
   3.showRecordInterface();
 * Input: cConf
 * Output:none
 * Return:none
 */
void ChannelDialog::setChannelConfig(ChannelConfig &cConf,ChannelDialog *cDialog)
{
    channelDialog = cDialog;
    v_cConf.mid = cConf.mid;
    v_cConf.cid = cConf.cid;
    mcrClient = MCRClientFactory::CreateInstance();
    //========
        QString aDir = QCoreApplication::applicationDirPath();
        aDir += "/config.ini";
        qWarning(aDir.toStdString().c_str());
        config = new QSettings(aDir,QSettings::IniFormat);
    //=========
    moduleServerUrl = config->value("/system/server").toString().toStdString();
    if(moduleServerUrl==""){
        moduleServerUrl = "http://127.0.0.1/soapserver.php";
    }
    showChannelInterface();
    showStreamInterface();
    showRecordInterface();
    alertLabel->setText("Loading ...... ");
    alertDlg->resize(400,200);
    alertDlg->show();
   //---------------------------------
    //open a pthread process load  config information
    checkConfInfoListThread = new CheckConfInfoListThread(this);
    checkConfInfoListThread->start();
    while(checkConfInfoListThread->isRunning()){
        QApplication::processEvents();
    }
    delete checkConfInfoListThread;
    checkConfInfoListThread = NULL;
    alertDlg->hide();

   //--------------------------------------------

}
/*
 * Function: on_btnChannel_clicked()
 * Description:this is a slot function.when you click the button ,it will
 * save channel config info into server.
 * Calls:
 * 1.setChannelConfig();
 * Input: cConf
 * Output:none
 * Return:none
 */
void ChannelDialog::on_btnChannel_clicked()
{

    ChannelConfig cConf;

    ErrorInfo e;
   QStandardItem* item;

    cConf.mid = v_cConf.mid;
    cConf.cid = v_cConf.cid;

    item = modelTablecConf->item(2,0);
    QString name = item->text();
    cConf.name = string((const char *)name.toUtf8());

    item = modelTablecConf->item(3,0);
    QString alias = item->text();
    cConf.alias = (const char*)alias.toUtf8();


    item = modelTablecConf->item(4,0);
    QString ip = item->text();

    if(ip == "") {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址格式不能为空！"));
        return ;
    }
    QString pattern("^([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");//ip模式

    QRegExp rx(pattern);
    if(!rx.exactMatch(ip)) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址格式不正确！"));
        return ;
    }
    cConf.ip = ip.toStdString();

    item = modelTablecConf->item(5,0);


    QString type = item->text();
    /*
    QString pattern2 = ("^[0-9]+$");
    QRegExp rx2(pattern2);
    if(!rx2.exactMatch(type)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("类型只能为数字"));
        return;
    }
    */
    if(type.compare("VGA") == 0) {
        cConf.type = 1;
    }else if (type.compare("AV") == 0){
        cConf.type = 2;
    }else if (type.compare("Audio") == 0) {
        cConf.type = 3;
    }


    item = modelTablecConf->item(6,0);
    QString streamUrl = item->text();
    cConf.streamUrl = streamUrl.toStdString();

    item = modelTablecConf->item(7,0);
    QString streamServerUrl = item->text();
    cConf.streamServerUrl = streamServerUrl.toStdString();

    item = modelTablecConf->item(8,0);
    QString recordServerUrl = item->text();
    cConf.recordServerUrl = recordServerUrl.toStdString();

    item = modelTablecConf->item(9,0);
    QString fileServerUrl = item->text();
    cConf.fileServerUrl = fileServerUrl.toStdString();



    mcrClient->setChannelConfig(&cConf,moduleServerUrl.c_str(),e);

    qDebug("e.result : %d\n",e.result);
    if(e.result == 0) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存成功"));
    }else {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存失败"));
    }
}
/*
 * Function: on_btnStream_clicked()
 * Description:this is a slot function.when you click the button ,it will
 * save stream config info into server.
 * Calls:
 * 1.setChannelStreamConfig();
 * Input: cConf
 * Output:none
 * Return:none
 */
void ChannelDialog::on_btnStream_clicked()
{
    StreamConfig sConf;

    ErrorInfo e;
   QStandardItem* item;


    //int i = str.toInt();
    //qDebug("i = %d\n",i);
    sConf.mid = v_cConf.mid;
    sConf.cid = v_cConf.cid;

    item = modelTablesConf->item(2,0);
    QString width = item->text();
    QString pattern("^[0-9]+$");
    QRegExp rx(pattern);
    if(!rx.exactMatch(width)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("视频宽度只能为数字"));
        return;
    }
    sConf.media_codec1_video_width = width.toInt();

    item = modelTablesConf->item(3,0);
    QString height = item->text();
    QString pattern1("^[0-9]+$");
    QRegExp rx1(pattern1);
    if(!rx1.exactMatch(height)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("视频高度只能为数字"));
        return;
    }
    sConf.media_codec1_video_height = height.toInt();

    item = modelTablesConf->item(4,0);
    QString v_bitrate = item->text();
    QString pattern2("^[0-9]+$");
    QRegExp rx2(pattern2);
    if(!rx2.exactMatch(v_bitrate)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("视频比特率只能为数字"));
        return;
    }
    sConf.media_codec1_video_bitrate = v_bitrate.toInt();

    item = modelTablesConf->item(5,0);
    QString  maxbitrate = item->text();
    QString pattern3("^[0-9]+$");
    QRegExp rx3(pattern3);
    if(!rx3.exactMatch(maxbitrate)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("视频最大比特率只能为数字"));
        return;
    }
    sConf.media_codec1_video_maxbitrate = maxbitrate.toInt();

    item = modelTablesConf->item(6,0);
    QString samplerate = item->text();
    QString pattern4("^[0-9]+$");
    QRegExp rx4(pattern4);
    if(!rx4.exactMatch(samplerate)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("音频采样速率只能为数字"));
        return;
    }
    sConf.media_capture_audio_samplerate = samplerate.toInt();

    item = modelTablesConf->item(7,0);
    QString a_bitrate = item->text();
    QString pattern5("^[0-9]+$");
    QRegExp rx5(pattern5);
    if(!rx5.exactMatch(a_bitrate)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("音频比特率只能为数字"));
        return;
    }
    sConf.media_codec1_audio_bitrate = a_bitrate.toInt();

    mcrClient->setChannelStreamConfig(&sConf,cConf.streamServerUrl.c_str(),e);

    qDebug("e.result : %d\n",e.result);
    if(e.result == 0) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存成功"));
    }else {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存失败"));
    }
}
/*
 * Function: on_btnRecord_clicked()
 * Description:this is a slot function.when you click the button ,it will
 * save record config info into server.
 * Calls:
 * 1.setChannelRecordConfig();
 * Input: cConf
 * Output:none
 * Return:none
 */
void ChannelDialog::on_btnRecord_clicked()
{
    RecordConfig rConf;

    ErrorInfo e;
   QStandardItem* item;


    //int i = str.toInt();
    //qDebug("i = %d\n",i);
    rConf.mid = v_cConf.mid;
    rConf.cid = v_cConf.cid;

    item = modelTablerConf->item(2,0);
    QString srcUrl = item->text();
    rConf.srcUrl = string((const char *)srcUrl.toLocal8Bit());

    item = modelTablerConf->item(3,0);

    QString prefix = item->text();

    QString pattern("^[0-9a-zA-Z_]+$");//匹配的模式
    QRegExp rx(pattern);
    if(!rx.exactMatch(prefix)) {//要匹配的输入字符串
       QMessageBox::information(this,QObject::tr("提示"),QObject::tr("前辍不能为中文且不为空"));
       return ;
    }
    rConf.prefix = string((const char *)prefix.toLocal8Bit());
    item = modelTablerConf->item(4,0);
    QString type = item->text();
    rConf.type = string((const char *)type.toLocal8Bit());
    item = modelTablerConf->item(5,0);
    QString segment = item->text();
    QString pattern3("^[0-9]+$");
    QRegExp rx3(pattern3);
    if(!rx3.exactMatch(segment)) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("分段只能为数字"));
        return ;
    }
    int segment_tmp = segment.toInt();
    if(30 <= segment_tmp && segment_tmp <= 3600) {
        rConf.segment = segment.toInt();
    }else {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("分段请输入30-3600之间的数"));
        return;
    }
    item = modelTablerConf->item(6,0);
    QString timeServer = item->text();

    if(timeServer == "") {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址不能为空！"));
        return ;
    }

    QString pattern2("^([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");//匹配的模式
    //QString pattern2("^((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]\d)|\d)(\.((25[0-5])|(2[0-4]\d)|(1\d\d)|([1-9]\d)|\d)){3}$");
    // QString pattern2("^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$");
    QRegExp rx2(pattern2);
    if(!rx2.exactMatch(timeServer)) {//要匹配的输入字符串
       QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址格式不正确！"));
       return ;
    }

    rConf.timeServer = string((const char*)timeServer.toLocal8Bit());



    mcrClient->setChannelRecordConfig(&rConf,cConf.recordServerUrl.c_str(),e);

    qDebug("e.result : %d\n",e.result);
    if(e.result == 0) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存成功"));
    }else {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存失败"));
    }
}

void CheckConfInfoListThread::run(){
    cdlg->showConfigInfo();
}

CheckConfInfoListThread::CheckConfInfoListThread(ChannelDialog *cdlg, QThread *in):
    QThread(in)
{
    this->cdlg = cdlg;
}
void ChannelDialog::showConfigInfo(){
    showChannelConfig();
    showStreamConfig();
    showRecordConfig();
}
/*
 * Function: showChannelInterface()
 * Description:this function is to show channel interface.
 * Calls:
 * Input: none
 * Output:none
 * Return:none
 */
void ChannelDialog::showChannelInterface(){
    modelTablecConf = new QStandardItemModel(10,0,ui->tableView_Channel);
    int j;

        for(j=0;j<10;j++){
            QStandardItem* item = new QStandardItem;

            modelTablecConf->setItem(j,0,item);
        }

    ui->tableView_Channel->setModel(modelTablecConf);

    ComboDelegate *comboDelegate;
    comboDelegate = new ComboDelegate;
    ui->tableView_Channel->setItemDelegateForRow(5,comboDelegate);

    modelTablecConf->setVerticalHeaderItem(0,new QStandardItem(QObject::tr("模块ID")));

    modelTablecConf->setVerticalHeaderItem(1,new QStandardItem(QObject::tr("信道ID")));
    modelTablecConf->setVerticalHeaderItem(2,new QStandardItem(QObject::tr("名        称")));
    modelTablecConf->setVerticalHeaderItem(3,new QStandardItem(QObject::tr("别        名")));
    modelTablecConf->setVerticalHeaderItem(4,new QStandardItem(QObject::tr("IP   地   址")));

    modelTablecConf->setVerticalHeaderItem(5,new QStandardItem(QObject::tr("类        型")));
    modelTablecConf->setVerticalHeaderItem(6,new QStandardItem(QObject::tr("流   地   址")));
    modelTablecConf->setVerticalHeaderItem(7,new QStandardItem(QObject::tr("流服务器地址")));
    modelTablecConf->setVerticalHeaderItem(8,new QStandardItem(QObject::tr("录制服务器地址")));
    modelTablecConf->setVerticalHeaderItem(9,new QStandardItem(QObject::tr("文件服务器地址")));


    ui->tableView_Channel->hideRow(0);
    ui->tableView_Channel->hideRow(1);
    ui->tableView_Channel->setRowHeight(2,30);
    ui->tableView_Channel->setRowHeight(3,30);
    ui->tableView_Channel->setRowHeight(4,30);
    ui->tableView_Channel->setRowHeight(5,30);
    ui->tableView_Channel->setRowHeight(6,30);
    ui->tableView_Channel->setRowHeight(7,30);
    ui->tableView_Channel->setRowHeight(8,30);
    ui->tableView_Channel->setRowHeight(9,30);

    ui->tableView_Channel->horizontalHeader()->hide();
    ui->tableView_Channel->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableView_Channel->setColumnWidth(0,250);
}
/*
 * Function: showStreamInterface()
 * Description:this function is to show stream interface.
 * Calls:
 * Input: none
 * Output:none
 * Return:none
 */
void ChannelDialog::showStreamInterface(){
    modelTablesConf = new QStandardItemModel(8,0,ui->tableView_Stream);
    int i;

    for(i=0;i<8;i++){
        QStandardItem* item = new QStandardItem;

        modelTablesConf->setItem(i,0,item);
    }

    ui->tableView_Stream->setModel(modelTablesConf);


    modelTablesConf->setVerticalHeaderItem(0,new QStandardItem(QObject::tr("模块ID")));
    modelTablesConf->setVerticalHeaderItem(1,new QStandardItem(QObject::tr("信道ID")));
    modelTablesConf->setVerticalHeaderItem(2,new QStandardItem(QObject::tr("视频宽度")));
    modelTablesConf->setVerticalHeaderItem(3,new QStandardItem(QObject::tr("视频高度")));
    modelTablesConf->setVerticalHeaderItem(4,new QStandardItem(QObject::tr("视频比特率")));
    modelTablesConf->setVerticalHeaderItem(5,new QStandardItem(QObject::tr("视频最大比特率")));
    modelTablesConf->setVerticalHeaderItem(6,new QStandardItem(QObject::tr("音频采样速率")));
    modelTablesConf->setVerticalHeaderItem(7,new QStandardItem(QObject::tr("音频比特率")));




    ui->tableView_Stream->setRowHeight(0,30);
    ui->tableView_Stream->setRowHeight(1,30);
    ui->tableView_Stream->setRowHeight(2,30);
    ui->tableView_Stream->setRowHeight(3,30);
    ui->tableView_Stream->setRowHeight(4,30);
    ui->tableView_Stream->setRowHeight(5,30);
    ui->tableView_Stream->setRowHeight(6,30);
    ui->tableView_Stream->setRowHeight(7,30);


    ui->tableView_Stream->hideRow(0);
    ui->tableView_Stream->hideRow(1);

    ui->tableView_Stream->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableView_Stream->horizontalHeader()->hide();
    ui->tableView_Stream->setColumnWidth(0,250);
}
/*
 * Function: showRecordInterface()
 * Description:this function is to show stream interface.
 * Calls:
 * Input: none
 * Output:none
 * Return:none
 */
void ChannelDialog::showRecordInterface(){
    modelTablerConf = new QStandardItemModel(7,0,ui->tableView_Record);

    int i;

    for(i=0;i<7;i++) {
            QStandardItem* item = new QStandardItem;
            modelTablerConf->setItem(i,0,item);
    }

    ui->tableView_Record->setModel(modelTablerConf);
    modelTablerConf->setVerticalHeaderItem(0,new QStandardItem(QObject::tr("模块ID")));
    modelTablerConf->setVerticalHeaderItem(1,new QStandardItem(QObject::tr("信道ID")));
    modelTablerConf->setVerticalHeaderItem(2,new QStandardItem(QObject::tr("源   地   址")));
    modelTablerConf->setVerticalHeaderItem(3,new QStandardItem(QObject::tr("文 件 前 缀")));
    modelTablerConf->setVerticalHeaderItem(4,new QStandardItem(QObject::tr("类        型")));
    modelTablerConf->setVerticalHeaderItem(5,new QStandardItem(QObject::tr("分        段")));
    modelTablerConf->setVerticalHeaderItem(6,new QStandardItem(QObject::tr("时间服务器IP")));


    ui->tableView_Record->hideRow(0);
    ui->tableView_Record->hideRow(1);
    ui->tableView_Record->hideRow(6);

    ui->tableView_Record->setRowHeight(2,30);

    ui->tableView_Record->setRowHeight(3,30);
    ui->tableView_Record->setRowHeight(4,30);

    ui->tableView_Record->setRowHeight(5,30);
    ui->tableView_Record->setRowHeight(6,30);

   // ui->tableView_Record->verticalHeader()->setStretchLastSection(true);
   // ui->tableView_Record->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_Record->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableView_Record->horizontalHeader()->hide();
    ui->tableView_Record->setColumnWidth(0,250);
}






void ChannelDialog::on_btnSaveAll_clicked()
{
    StreamConfig sConf;
    RecordConfig rConf;
    ChannelConfig cConf;
    ErrorInfo e;
    QStandardItem* item;

   // QString pattern,pattern1,pattern2,pattern3,pattern4, \
            pattern5,pattern6,pattern7,pattern8;
 //   QRegExp rx,rx1,rx2,rx3,rx4,rx5,rx6,rx7,rx8;
    cConf.mid = v_cConf.mid;
    cConf.cid = v_cConf.cid;

    item = modelTablecConf->item(2,0);
    QString name = item->text();
    cConf.name = string((const char *)name.toUtf8());

    item = modelTablecConf->item(3,0);
    QString alias = item->text();
    cConf.alias = (const char*)alias.data();


    item = modelTablecConf->item(4,0);
    QString ip = item->text();

    if(ip == "") {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址格式不能为空！"));
        return ;
    }
    QString pattern9("^([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");//ip模式

    QRegExp rx9(pattern9);
    if(!rx9.exactMatch(ip)) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址格式不正确！"));
        return ;
    }
    cConf.ip = ip.toStdString();

    item = modelTablecConf->item(5,0);


    QString type = item->text();
    /*
    QString pattern2 = ("^[0-9]+$");
    QRegExp rx2(pattern2);
    if(!rx2.exactMatch(type)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("类型只能为数字"));
        return;
    }
    */
    if(type.compare("VGA") == 0) {
        cConf.type = 1;
    }else if (type.compare("AV") == 0){
        cConf.type = 2;
    }else if (type.compare("Audio") == 0) {
        cConf.type = 3;
    }


    item = modelTablecConf->item(6,0);
    QString streamUrl = item->text();
    cConf.streamUrl = streamUrl.toStdString();

    item = modelTablecConf->item(7,0);
    QString streamServerUrl = item->text();
    cConf.streamServerUrl = streamServerUrl.toStdString();

    item = modelTablecConf->item(8,0);
    QString recordServerUrl = item->text();
    cConf.recordServerUrl = recordServerUrl.toStdString();

    item = modelTablecConf->item(9,0);
    QString fileServerUrl = item->text();
    cConf.fileServerUrl = fileServerUrl.toStdString();

    //获取当前模块的IP
    QString aDir = QCoreApplication::applicationDirPath();
    aDir += "/module_"+ QString::number(cConf.mid) +".ini";
    qWarning(aDir.toStdString().c_str());
    config = new QSettings(aDir,QSettings::IniFormat);

     std::string moduleServerUrl;

    moduleServerUrl = "http://";
    moduleServerUrl += config->value("/network/ip").toString().toStdString();
    moduleServerUrl +=  "/soapserver.php";

   // mcrClient->setChannelConfig(&cConf,moduleServerUrl.c_str(),e);

    qDebug("e.result : %d\n",e.result);
    /*
    if(e.result == 0) {
        //QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存成功"));
    }else {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存通道参数失败"));
        return;
    }
    */
//-----------------------------------
    //int i = str.toInt();
    //qDebug("i = %d\n",i);
    rConf.mid = v_cConf.mid;
    rConf.cid = v_cConf.cid;

    item = modelTablerConf->item(2,0);
    QString srcUrl = item->text();
    rConf.srcUrl = string((const char *)srcUrl.toLocal8Bit());

    item = modelTablerConf->item(3,0);

    QString prefix = item->text();

    QString pattern("^[0-9a-zA-Z_]+$");//匹配的模式
    QRegExp rx(pattern);
    if(!rx.exactMatch(prefix)) {//要匹配的输入字符串
       QMessageBox::information(this,QObject::tr("提示"),QObject::tr("前辍不能为中文且不为空"));
       return ;
    }
    rConf.prefix = string((const char *)prefix.toLocal8Bit());
    item = modelTablerConf->item(4,0);
    type = item->text();
    rConf.type = string((const char *)type.toLocal8Bit());
    item = modelTablerConf->item(5,0);
    QString segment = item->text();
    QString pattern1("^[0-9]+$");
    QRegExp rx1(pattern1);
    if(!rx1.exactMatch(segment)) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("分段只能为数字"));
        return ;
    }
    int segment_tmp = segment.toInt();
    if(30 <= segment_tmp && segment_tmp <= 1200) {
        rConf.segment = segment.toInt();
    }else {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("分段请输入30-1200之间的数"));
        return;
    }
    item = modelTablerConf->item(6,0);
    QString timeServer = item->text();

    if(timeServer == "") {
//        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址不能为空！"));
//        return ;
    }

//    QString pattern2("^([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\.([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\.([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\.([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");//匹配的模式

//    QRegExp rx2(pattern2);
//    if(!rx2.exactMatch(timeServer)) {//要匹配的输入字符串
//       QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址格式不正确！"));
//       return ;
//    }

    rConf.timeServer = string((const char*)timeServer.toLocal8Bit());



    mcrClient->setChannelRecordConfig(&rConf,cConf.recordServerUrl.c_str(),e);

    qDebug("e.result : %d\n",e.result);
    if(e.result == 0) {
       // QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存成功"));
    }else {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存录制参数失败"));
    }
//------------------------------------
    //int i = str.toInt();
    //qDebug("i = %d\n",i);
    sConf.mid = v_cConf.mid;
    sConf.cid = v_cConf.cid;

    item = modelTablesConf->item(2,0);
    QString width = item->text();
   QString pattern3("^[0-9]+$");
   QRegExp rx3(pattern3);
    if(!rx3.exactMatch(width)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("视频宽度只能为数字"));
        return;
    }
    sConf.media_codec1_video_width = width.toInt();

    item = modelTablesConf->item(3,0);
    QString height = item->text();
    QString pattern4("^[0-9]+$");
    QRegExp rx4(pattern4);
    if(!rx4.exactMatch(height)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("视频高度只能为数字"));
        return;
    }
    sConf.media_codec1_video_height = height.toInt();

    item = modelTablesConf->item(4,0);
    QString v_bitrate = item->text();
    QString pattern5("^[0-9]+$");
    QRegExp rx5(pattern5);
    if(!rx5.exactMatch(v_bitrate)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("视频比特率只能为数字"));
        return;
    }
    sConf.media_codec1_video_bitrate = v_bitrate.toInt();

    item = modelTablesConf->item(5,0);
    QString  maxbitrate = item->text();
    QString pattern6("^[0-9]+$");
    QRegExp rx6(pattern6);
    if(!rx6.exactMatch(maxbitrate)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("视频最大比特率只能为数字"));
        return;
    }
    sConf.media_codec1_video_maxbitrate = maxbitrate.toInt();

    item = modelTablesConf->item(6,0);
    QString samplerate = item->text();
    QString pattern7("^[0-9]+$");
    QRegExp rx7(pattern7);
    if(!rx7.exactMatch(samplerate)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("音频采样速率只能为数字"));
        return;
    }
    sConf.media_capture_audio_samplerate = samplerate.toInt();

    item = modelTablesConf->item(7,0);
    QString a_bitrate = item->text();
    QString pattern8("^[0-9]+$");
    QRegExp rx8(pattern8);
    if(!rx8.exactMatch(a_bitrate)){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("音频比特率只能为数字"));
        return;
    }
    sConf.media_codec1_audio_bitrate = a_bitrate.toInt();

    mcrClient->setChannelStreamConfig(&sConf,cConf.streamServerUrl.c_str(),e);

    qDebug("e.result : %d\n",e.result);
    if(e.result == 0) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存成功"));
        channelDialog->close();
    }else {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存流参数失败"));
    }
}
