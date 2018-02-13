#include "moduledialog.h"
#include "ui_moduledialog.h"


#include"mcrclient.h"
#include<QMessageBox>
#include "dialog.h"
#include"moduleitemdelegate.h"
#include<string.h>



ModuleDialog::ModuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModuleDialog)
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
    //config = new QSettings("config.ini",QSettings::IniFormat);
    moduleServerUrl = config->value("/system/server").toString().toStdString();
    if(moduleServerUrl==""){
        moduleServerUrl = "http://127.0.0.1/soapserver.php";
    }

}

ModuleDialog::~ModuleDialog()
{   
    delete ui;
}
/*
 * Function: showModuleConfig()
 * Description:this function is to get module config information.
 * and show it on the interface.
 * Calls:
 * 1.getModuleConfig();
 * Input: none
 * Output:none
 * Return:none
 */
void ModuleDialog::showModuleConfig()
{
   QStandardItem* item;
   int Mid;
   int i;
   ErrorInfo e;

   Mid = v_mConf.mid;
/*
   mcrClient->getModuleConfig(mConf, Mid,moduleServerUrl.c_str(), e);

   if(e.result != 0) {
       qWarning("get module config error\n");
   }
   */

   QString mDir = QCoreApplication::applicationDirPath();
   QSettings *mConfig;
   mDir += "/module_"+QString::number(Mid) +".ini";
   qWarning(mDir.toStdString().c_str());
   mConfig = new QSettings(mDir,QSettings::IniFormat);
  // moduleServerUrl = mConfig->value("//").toString().toStdString();
    mConf.mid = mConfig->value("/description/mid").toInt();
    mConf.alias = mConfig->value("/description/alias").toString().toStdString();
    mConf.name = mConfig->value("/description/name").toString().toStdString();
    mConf.ip = mConfig->value("/network/ip").toString().toStdString();
    QString qstr;
    qstr = "通道";
   qWarning("get module config successful\n");
   for(i=0;i<5;i++) {
    item  = modelTablemConf->item(i,0);

    if(i == 0) //mid
    item->setText(QString::number(mConf.mid));
   // if(i == 1) //channelCount
    //item->setText(QString::number(mConf.cidArray.size()));
    if(i == 2) //name
    item->setText(QObject::tr(mConf.name.c_str()));
    if(i == 3) //alias
    item->setText(QObject::tr(mConf.alias.c_str()));
    if(i == 4) //ip
    item->setText(tr(mConf.ip.c_str()));
    item->setEnabled(false);
   }

//========================================================

}


/*
 * Function: showModuleConfig()
 * Description:this function is to get module config information
 * from the treeView and show it on the interface.
 * Calls:
 * 1.showModuleInterface();
 * Input: none
 * Output:none
 * Return:none
 */
void ModuleDialog::setModuleConfig(ModuleConfig &mConf,ModuleDialog *mDialog)
{
    moduleDialog = mDialog;
    v_mConf.mid = mConf.mid;
   // v_mConf.cidArray = mConf.cidArray;

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
    checkModuleConfInfoThread = new CheckModuleConfInfoThread(this);
    checkModuleConfInfoThread->start();
    while(checkModuleConfInfoThread->isRunning()){
        QApplication::processEvents();
    }
    delete checkModuleConfInfoThread;
    checkModuleConfInfoThread = NULL;
    alertDlg->hide();

   //--------------------------------------------

}
/*
 * Function: on_btnModule_clicked()
 * Description:this is a slot function.when you click the button you will
 * send the module config info to server.
 * Calls:
 * 1.setModuleConfig();
 * Input: none
 * Output:none
 * Return:none
 */
void ModuleDialog::on_btnModule_clicked()
{

    ModuleConfig mConf;

    ErrorInfo e;
    QStandardItem* item;

    mConf.mid = v_mConf.mid;
   // mConf.cidArray = v_mConf.cidArray;

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

    if(!rx.exactMatch(ip)) {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址格式不正确！"));
        return ;
    }

    mConf.ip = ip.toStdString();


    QString mDir = QCoreApplication::applicationDirPath();
    mDir += "/module_"+QString::number(mConf.mid)+".ini";
    qWarning(mDir.toStdString().c_str());

    QSettings* mConfig;
    mConfig = new QSettings(mDir,QSettings::IniFormat);

    mConfig->setValue("description/mid",mConf.mid);
    mConfig->setValue("/description/alias",QString::fromStdString(mConf.alias));
    mConfig->setValue("/description/name",QString::fromStdString(mConf.name));
    mConfig->setValue("network/ip",QString::fromStdString(mConf.ip));
    QMessageBox::information(this,QObject::tr("提示"),QObject::tr("保存成功"));
    moduleDialog->close();
}





void CheckModuleConfInfoThread::run(){
    mdlg->showConfigInfo();
}

CheckModuleConfInfoThread::CheckModuleConfInfoThread(ModuleDialog *mdlg, QThread *in):
    QThread(in)
{
    this->mdlg = mdlg;
}
void ModuleDialog::showConfigInfo(){
    showModuleConfig();
}
/*
 * Function: showModuleInterface()
 * Description:this function is to show the interface of module.
 * Calls:
 * Input: none
 * Output:none
 * Return:none
 */
void ModuleDialog::showModuleInterface(){
    modelTablemConf = new QStandardItemModel(5,0,ui->tableView_Module);
    int j;

        for(j=0;j<5;j++){
            QStandardItem* item = new QStandardItem;

            modelTablemConf->setItem(j,0,item);
        }

    ui->tableView_Module->setModel(modelTablemConf);


    modelTablemConf->setVerticalHeaderItem(0,new QStandardItem(QObject::tr("模块ID")));
    modelTablemConf->setVerticalHeaderItem(1,new QStandardItem(QObject::tr("通道数")));
    modelTablemConf->setVerticalHeaderItem(2,new QStandardItem(QObject::tr("模块名称")));
    modelTablemConf->setVerticalHeaderItem(3,new QStandardItem(QObject::tr("模块别名")));
    modelTablemConf->setVerticalHeaderItem(4,new QStandardItem(QObject::tr("模块IP")));





    ui->tableView_Module->setRowHeight(2,30);
    ui->tableView_Module->setRowHeight(3,30);
    ui->tableView_Module->setRowHeight(4,30);




    ui->tableView_Module->hideRow(0);
    ui->tableView_Module->hideRow(1);
    ui->tableView_Module->horizontalHeader()->hide();
    ui->tableView_Module->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableView_Module->setColumnWidth(0,500);
}








void ModuleDialog::on_btn_RebootModule_clicked()
{
    std::string module_IP = "";
    ErrorInfo e;

    if(mConf.ip == ""){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址为空，无法重新启动"));
        return;
    }

    QMessageBox box;
    box.setWindowTitle(QObject::tr("提示"));
    box.setIcon(QMessageBox::Information);
    box.setText(QObject::tr("确认是否要重启?"));

    box.setStandardButtons(QMessageBox::Yes|QMessageBox::Cancel);
    box.setButtonText(QMessageBox::Yes,QString(tr("确定")));
    box.setButtonText(QMessageBox::Cancel,QString(tr("取消")));
    int nRet = box.exec();
    if(nRet == QMessageBox::Yes){
        module_IP += "http://";
        module_IP += mConf.ip;
        module_IP += "/soapserver.php";
        mcrClient->reboot(module_IP.c_str(),e);
        if(e.result == 0){
            QMessageBox::information(this,QObject::tr("提示"),QObject::tr("模块重启成功，大约60秒后重启完毕"));
            this->close();
        }else{
            QMessageBox::information(this,QObject::tr("提示"),QObject::tr("模块重启失败"));
        }
    }else{

    }
}




