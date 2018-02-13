#include "addmoduledialog.h"
#include "ui_addmoduledialog.h"


#include"mcrclient.h"
#include<QMessageBox>

#include"moduleitemdelegate.h"
#include<string.h>



AddModuleDialog::AddModuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddModuleDialog)
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

    moduleServerUrl = config->value("/system/server").toString().toStdString();
    if(moduleServerUrl==""){
        moduleServerUrl = "http://127.0.0.1/soapserver.php";
    }





}

AddModuleDialog::~AddModuleDialog()
{   
    delete ui;
}
/*
 * Function: showModuleConfig()
 * Description:this function is to init the module config data and
 * show it on the interface.
 * Calls: none
 * Input: none
 * Output:none
 * Return:none
 */
void AddModuleDialog::showModuleConfig()
{



   QStandardItem* item;
   int i;

   ErrorInfo e;


   for(i=0;i<5;i++) {
    item  = modelTablemConf->item(i,0);

    if(i == 0) //channelCount
     item->setText(QString::number(0));
    if(i == 1) //mid
     item->setText(QString::number(0));
    if(i == 2) //name
     item->setText(tr("module"));
    if(i == 3) //alias
     item->setText(tr("模块"));
    if(i == 4) //ip
     item->setText(tr("192.168.0.17"));

   }

}
/*
 * Function: setModuleConfig()
 * Description:this function is to show module config data on
 * the interface.
 * Calls:
 * 1.showModuleInterface();
 * Input: none
 * Output:none
 * Return:none
 */
void AddModuleDialog::setModuleConfig(AddModuleDialog *addMDialog)
{

    addModuleDialog = addMDialog;
    mcrClient = MCRClientFactory::CreateInstance();
    moduleServerUrl = config->value("/system/server").toString().toStdString();
    if(moduleServerUrl==""){
        moduleServerUrl = "http://127.0.0.1/soapserver.php";
    }

    showModuleInterface();


    alertLabel->setText("Loading ...... ");
    alertDlg->resize(400,200);
    alertDlg->show();
   //---------------------------------
    //open a pthread process load  config information
    checkAddModuleConfInfoThread = new CheckAddModuleConfInfoThread(this);
    checkAddModuleConfInfoThread->start();
    while(checkAddModuleConfInfoThread->isRunning()){
        QApplication::processEvents();
    }
    delete checkAddModuleConfInfoThread;
    checkAddModuleConfInfoThread = NULL;
    alertDlg->hide();

   //--------------------------------------------

}
void CheckAddModuleConfInfoThread::run(){
    amdlg->showConfigInfo();
}

CheckAddModuleConfInfoThread::CheckAddModuleConfInfoThread(AddModuleDialog *amdlg, QThread *in):
    QThread(in)
{
    this->amdlg = amdlg;
}
void AddModuleDialog::showConfigInfo(){
    showModuleConfig();
}
/*
 * Function: showModuleInterface()
 * Description:this function is to show module config interface.
 * Calls:
 * 1.showModuleInterface();
 * Input: none
 * Output:none
 * Return:none
 */
void AddModuleDialog::showModuleInterface(){
    modelTablemConf = new QStandardItemModel(5,0,ui->tableView_Module);
    int j;

        for(j=0;j<5;j++){
            QStandardItem* item = new QStandardItem;

            modelTablemConf->setItem(j,0,item);
        }

    ui->tableView_Module->setModel(modelTablemConf);



    modelTablemConf->setVerticalHeaderItem(0,new QStandardItem(QObject::tr("通道数")));
     modelTablemConf->setVerticalHeaderItem(1,new QStandardItem(QObject::tr("模块ID")));
    modelTablemConf->setVerticalHeaderItem(2,new QStandardItem(QObject::tr("模块名称")));
    modelTablemConf->setVerticalHeaderItem(3,new QStandardItem(QObject::tr("模块别名")));
    modelTablemConf->setVerticalHeaderItem(4,new QStandardItem(QObject::tr("模块IP")));
    ui->tableView_Module->setRowHeight(0,30);
    ui->tableView_Module->setRowHeight(1,30);
    ui->tableView_Module->setRowHeight(2,30);
    ui->tableView_Module->setRowHeight(3,30);
    ui->tableView_Module->setRowHeight(4,30);

    ui->tableView_Module->hideRow(0);
    ui->tableView_Module->hideRow(1);
    ui->tableView_Module->horizontalHeader()->hide();
    ui->tableView_Module->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableView_Module->setColumnWidth(0,500);
}
/*
 * Function: on_btn_AddModule_clicked()
 * Description:this is a slot function.when you click the button
 * you will send the module config info to the server and add a module.
 * Calls:
 * 1.addModule();
 * Input: none
 * Output:none
 * Return:none
 */
void AddModuleDialog::on_btn_AddModule_clicked()
{
    ModuleConfig mConf;

    ErrorInfo e;
    QStandardItem* item;

    flag = 0;
    mConf.mid = addMid();

  //  mConf.cidArray = v_mConf.cidArray;

    item = modelTablemConf->item(2,0);
    QString name = item->text();
    mConf.name = string((const char *)name.toUtf8());

    item = modelTablemConf->item(3,0);
    QString alias = item->text();

    mConf.alias = string((const char *)alias.toUtf8());


    item = modelTablemConf->item(4,0);
    QString ip = item->text();

    if(ip == "") {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址格式不能为空！"));
        return ;
    }
    QString pattern("^([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");//ip模式

    QRegExp rx(pattern);
/*
    if(!rx.exactMatch(ip)) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址格式不正确！"));
        return ;
    }
*/
    mConf.ip = ip.toStdString();

    QSettings *mConfig;
    QString mDir = QCoreApplication::applicationDirPath();
    mDir += "/module_"+QString::number(mConf.mid)+".ini";
    qWarning(mDir.toStdString().c_str());
    mConfig = new QSettings(mDir,QSettings::IniFormat);

    mConfig->setValue("/description/mid",mConf.mid);
    mConfig->setValue("/description/name",QString::fromStdString(mConf.name));
    mConfig->setValue("/description/alias",QString::fromStdString(mConf.alias));
    mConfig->setValue("/network/ip",QString::fromStdString(mConf.ip));

    QMessageBox::information(this,QObject::tr("提示"),QObject::tr("增加模块成功"));
    flag = 1;
    addModuleDialog->close();
}
int AddModuleDialog::addMid()
{
    QSettings *mCount;
    std::string mid_count;
    std::string insert_mid = "";
    QVector<int> tmp_mid;
    QVector<int>::iterator it;

    int max_count;
    int new_mid = -1 ;
    QString tmp = "";
    const char* pmid = NULL;
    QString mDir = QCoreApplication::applicationDirPath();
    mDir += "/module_count.ini";
    qWarning(mDir.toStdString().c_str());
    mCount = new QSettings(mDir,QSettings::IniFormat);
    mid_count = mCount->value("/count/count").toString().toStdString();
    pmid = mid_count.c_str();
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
    max_count = tmp_mid.count();
    it = tmp_mid.begin();
    for(int i=0;i<=max_count;i++,it++){
        if(i != *it && it != tmp_mid.end()){
            new_mid = i;
            break;
        }else if(i == max_count){
            new_mid = i;
            break;
        }
    }

    tmp_mid.insert(max_count,new_mid);
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


    return new_mid;
}


