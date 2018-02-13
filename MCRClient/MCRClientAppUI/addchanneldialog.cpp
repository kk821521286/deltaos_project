#include "addchanneldialog.h"
#include "ui_addchanneldialog.h"


#include"mcrclient.h"

#include"combodelegate.h"
#include<QMessageBox>
#include<QComboBox>
#include"moduleitemdelegate.h"
#include<string.h>



AddChannelDialog::AddChannelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddChannelDialog)
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

    flag = 0;
}

AddChannelDialog::~AddChannelDialog()
{   
    delete ui;
}
/*
 * Function: showChannelConfig()
 * Description:this function is to set the initial config data
 * and show it on the interface.
 * Calls:
 * Input: none
 * Output:none
 * Return:none
 */
void AddChannelDialog::showChannelConfig()
{



    QStandardItem* item;
    int i;
    ErrorInfo e;

    ChannelInfoData channelInfoData;

    channelInfoData = v_cInfoData;

   for(i=0;i<10;i++) {
    item  = modelTablecConf->item(i,0);
    if(i == 0) //mid
    item->setText(QString::number(channelInfoData.channelConfig.mid));
    if(i == 1) //cid
    item->setText(QString::number(0));
    if(i == 2) //name
    item->setText(QObject::tr("channel"));
    if(i == 3) //alias
    item->setText(QObject::tr("通道"));
    if(i == 4) //ip
    item->setText(tr("192.168.0.220"));
    if(i == 5){//type
        item->setText(tr("VGA"));
    }
    if(i == 6)//streamUrl
    item->setText(QObject::tr("rtsp://192.168.0.220:8554/test-0"));
    if(i == 7)//streamServerUrl
    item->setText(QObject::tr("http://192.168.0.220:5536"));
    if(i == 8)//recordServerUrl
    item->setText(QObject::tr("http://192.168.0.201/soapserver.php"));
    if(i == 9)//fileServerUrl
    item->setText(QObject::tr("http://192.168.0.201/soapserver.php"));
   }

//========================================================



}


/*
 * Function: setChannelConfig()
 * Description:this function is to get the channel config info
 * and show it on the interface.
 * Calls:
 * 1.showChannelInterface();
 * Input: none
 * Output:none
 * Return:none
 */
void AddChannelDialog::setChannelConfig(std::string &serverUrl,ChannelInfoData &cInfoData,AddChannelDialog *addCDialog)
{

    addChannelDlg = addCDialog;
    mcrClient = MCRClientFactory::CreateInstance();
   // moduleServerUrl = config->value("/system/server").toString().toStdString();
    moduleServerUrl = "http://"+serverUrl+"/soapserver.php";
    //    if(moduleServerUrl==""){
//        moduleServerUrl = "http://127.0.0.1/soapserver.php";
//    }

    showChannelInterface();
    v_cInfoData = cInfoData;
    alertLabel->setText("Loading ...... ");
    alertDlg->resize(400,200);
    alertDlg->show();
   //---------------------------------
    //open a pthread process load  config information
    checkAddChannelConfInfoThread = new CheckAddChannelConfInfoThread(this);
    checkAddChannelConfInfoThread->start();
    while(checkAddChannelConfInfoThread->isRunning()){
        QApplication::processEvents();
    }
    delete checkAddChannelConfInfoThread;
    checkAddChannelConfInfoThread = NULL;
    alertDlg->hide();

   //--------------------------------------------

}

void CheckAddChannelConfInfoThread::run(){
    acdlg->showConfigInfo();
}

CheckAddChannelConfInfoThread::CheckAddChannelConfInfoThread(AddChannelDialog *acdlg, QThread *in):
    QThread(in)
{
    this->acdlg = acdlg;
}
void AddChannelDialog::showConfigInfo(){
    showChannelConfig();
    showChannelRecordConfig();
}
/*
 * Function: showChannelInterface()
 * Description:this function is to show channel interface.
 * Calls:
 * Input: none
 * Output:none
 * Return:none
 */
void AddChannelDialog::showChannelInterface(){
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
    modelTablecConf->setVerticalHeaderItem(1,new QStandardItem(QObject::tr("通道ID")));
    modelTablecConf->setVerticalHeaderItem(2,new QStandardItem(QObject::tr("通道名称")));
    modelTablecConf->setVerticalHeaderItem(3,new QStandardItem(QObject::tr("通道别名")));
    modelTablecConf->setVerticalHeaderItem(4,new QStandardItem(QObject::tr("通道IP")));
    modelTablecConf->setVerticalHeaderItem(5,new QStandardItem(QObject::tr("类型")));

    modelTablecConf->setVerticalHeaderItem(6,new QStandardItem(QObject::tr("流地址")));
    modelTablecConf->setVerticalHeaderItem(7,new QStandardItem(QObject::tr("流服务器地址")));
    modelTablecConf->setVerticalHeaderItem(8,new QStandardItem(QObject::tr("记录服务器地址")));
    modelTablecConf->setVerticalHeaderItem(9,new QStandardItem(QObject::tr("文件服务器地址")));





    ui->tableView_Channel->setRowHeight(0,30);
    ui->tableView_Channel->setRowHeight(1,30);
    ui->tableView_Channel->setRowHeight(2,30);
    ui->tableView_Channel->setRowHeight(3,30);
    ui->tableView_Channel->setRowHeight(4,30);
    ui->tableView_Channel->setRowHeight(5,30);
    ui->tableView_Channel->setRowHeight(6,30);
    ui->tableView_Channel->setRowHeight(7,30);
    ui->tableView_Channel->setRowHeight(8,30);
    ui->tableView_Channel->setRowHeight(9,30);






    ui->tableView_Channel->setRowHidden(0,true);
    ui->tableView_Channel->horizontalHeader()->hide();
    ui->tableView_Channel->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableView_Channel->setColumnWidth(0,250);

    showChannelRecordInterface();
}
/*
 * Function: on_btn__AddChannel_clicked()
 * Description:this is a slot function.when you click the button
 * you will send channel info to the server and add a channel.
 * Calls:
 * 1.addChannel();
 * Input: none
 * Output:none
 * Return:none
 */
void AddChannelDialog::on_btn__AddChannel_clicked()
{
    ChannelConfig cConf;

    RecordConfig rConf;
    ErrorInfo e;
   QStandardItem* item;

    //std::string serverUrl = "http://"+v_cInfoData.channelConfig.ip
    flag = 0;
    cConf.mid = v_cInfoData.channelConfig.mid;
    cConf.cid = 0;

    item = modelTablecConf->item(2,0);
    QString name = item->text();
    cConf.name = string((const char *)name.toUtf8());

    item = modelTablecConf->item(3,0);
    QString alias = item->text();
    cConf.alias = string((const char *)alias.toUtf8());


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
    if(type.compare("VGA") == 0) {
        qDebug("~");
        cConf.type = 1;
    }else if (type.compare("AV") == 0) {
        qDebug("~~");
        cConf.type = 2;
    }else if (type.compare("Audio") == 0){
        qDebug("~~~");
        cConf.type = 3;
    }else{
        cConf.type = 1;
    }


    item = modelTablecConf->item(6,0);
    QString streamUrl = item->text();
    cConf.streamUrl = string((const char *)streamUrl.toUtf8());

    item = modelTablecConf->item(7,0);
    QString streamServerUrl = item->text();
    cConf.streamServerUrl = string((const char *)streamServerUrl.toUtf8());

    item = modelTablecConf->item(8,0);
    QString recordServerUrl = item->text();
    cConf.recordServerUrl = string((const char *)recordServerUrl.toUtf8());

    item = modelTablecConf->item(9,0);
    QString fileServerUrl = item->text();
    cConf.fileServerUrl = string((const char *)fileServerUrl.toUtf8());

//record config info of add channel
    rConf.mid = v_cInfoData.channelConfig.mid;
    rConf.cid = 0;

    item = modelTablerConf->item(2,0);
    QString srcUrl = item->text();
    rConf.srcUrl = string((const char *)srcUrl.toUtf8());

    item = modelTablerConf->item(3,0);
    QString prefix = item->text();
    rConf.prefix = string((const char *)prefix.toUtf8());

    item = modelTablerConf->item(4,0);
    QString type1 = item->text();
    rConf.type = string((const char *)type1.toUtf8());

    item = modelTablerConf->item(5,0);
    QString segment = item->text();
    rConf.segment = segment.toInt();

    item = modelTablerConf->item(6,0);
    QString timeServer = item->text();
    rConf.timeServer = string((const char *)timeServer.toUtf8());

    int get_cid = 0;
    mcrClient->addChannel(&cConf,get_cid,moduleServerUrl.c_str(),e);

    if(e.result != 0){
      QMessageBox::information(this,QObject::tr("提示"),QObject::tr("新增通道失败"));
      return;
    }
    //从addChannel()接口中获取一个cid
    rConf.cid = get_cid;
    mcrClient->addChannelRecord(&rConf,moduleServerUrl.c_str(),e);
    qDebug("add channel successful");
    qDebug("e.result : %d\n",e.result);
    if(e.result == 0) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("新增通道成功,需重启本模块后该通道才能正常工作"));
        flag = 1;
        ui->btn__AddChannel->setDisabled(true);
        addChannelDlg->close();
    }else {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("新增通道失败"));
    }
}
void AddChannelDialog::showChannelRecordInterface()
{
    modelTablerConf = new QStandardItemModel(7,0,ui->tableView_RecordChannel);

    int i;

    for(i=0;i<7;i++) {

            QStandardItem* item = new QStandardItem;
            modelTablerConf->setItem(i,0,item);
    }

    ui->tableView_RecordChannel->setModel(modelTablerConf);
    modelTablerConf->setVerticalHeaderItem(0,new QStandardItem(QObject::tr("模块ID")));
    modelTablerConf->setVerticalHeaderItem(1,new QStandardItem(QObject::tr("信道ID")));
    modelTablerConf->setVerticalHeaderItem(2,new QStandardItem(QObject::tr("源   地   址")));
    modelTablerConf->setVerticalHeaderItem(3,new QStandardItem(QObject::tr("文 件 前 缀")));
    modelTablerConf->setVerticalHeaderItem(4,new QStandardItem(QObject::tr("类        型")));
    modelTablerConf->setVerticalHeaderItem(5,new QStandardItem(QObject::tr("分        段")));
    modelTablerConf->setVerticalHeaderItem(6,new QStandardItem(QObject::tr("时间服务器IP")));


    ui->tableView_RecordChannel->hideRow(0);
    ui->tableView_RecordChannel->hideRow(1);
    ui->tableView_RecordChannel->hideRow(6);

    ui->tableView_RecordChannel->setRowHeight(2,30);
    ui->tableView_RecordChannel->setRowHeight(3,30);
    ui->tableView_RecordChannel->setRowHeight(4,30);
    ui->tableView_RecordChannel->setRowHeight(5,30);
    ui->tableView_RecordChannel->setRowHeight(6,30);

   // ui->tableView_Record->verticalHeader()->setStretchLastSection(true);
   // ui->tableView_Record->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_RecordChannel->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableView_RecordChannel->horizontalHeader()->hide();
    ui->tableView_RecordChannel->setColumnWidth(0,250);
}


void AddChannelDialog::showChannelRecordConfig()
{
    QStandardItem* item;
    int i;

    ChannelInfoData channelInfoData;

    channelInfoData = v_cInfoData;

   for(i=0;i<10;i++) {
    item  = modelTablerConf->item(i,0);
    if(i == 0) //mid
    item->setText(QString::number(channelInfoData.channelConfig.mid));
    if(i == 1) //cid
    item->setText(QString::number(0));
    if(i == 2)//srcUrl
    item->setText(QObject::tr("rtsp://192.168.0.220:8554/test-0"));
    if(i == 3) //prefix
    item->setText(QObject::tr("stream"));
    if(i == 4) //type
    item->setText(QObject::tr("mp4"));
    if(i == 5) //segment
    item->setText(tr("1200"));
    if(i == 6){//timeServer
        item->setText(tr("192.168.0.233"));
    }
    }
}
