#include "updatedialog.h"
#include "ui_updatedialog.h"
#include "updateipdelegate.h"
#include <QSettings>
#include <QDesktopServices>
#include <QFileDialog>
#include <QDateTime>
#include <unistd.h>
#include "mcrclientqt.h"
#include <QMessageBox>
UpdateDialog::UpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateDialog)
{
          ui->setupUi(this);
         QString aDir = QCoreApplication::applicationDirPath();
         aDir += "/config.ini";
         qWarning(aDir.toStdString().c_str());
         config = new QSettings(aDir,QSettings::IniFormat);
         moduleServerUrl = config->value("/system/server").toString().toStdString();
         if(moduleServerUrl==""){
             moduleServerUrl = "http://127.0.0.1/soapserver.php";
         }
         mcrClient = MCRClientFactory::CreateInstance();

         modelTableIpConf = new QStandardItemModel(0,0,ui->listView);

         QStandardItem* item = new QStandardItem;

         modelTableIpConf->setItem(0,0,item);

         ui->listView->setFixedHeight(20);
         ui->listView->setFixedWidth(120);
         ui->listView->setModel(modelTableIpConf);
         UpdateIpDelegate *updateIpDelegate = new UpdateIpDelegate;

         ui->listView->setItemDelegateForRow(0,updateIpDelegate);
         //初始化上传按钮为disable
         ui->pushButton_upload->setEnabled(false);
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::on_pushButton_browser_clicked()
{

    QString path = "";
    char  *ptr = NULL;
   //显示当前的升级模块
    QStandardItem *item =  modelTableIpConf->item(0,0);

    QString strIP = item->text();
    if("" != strIP){
        ui->lineEdit_IP->setText(strIP);
    }else{
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请选择升级系统IP"));
        return;
    }
    //选择上传文件前，清空之前的内容
    if("" != ui->label_file->text())
    {
        ui->label_file->setText("");
    }
    if("" != ui->label_upload->text()){
        ui->label_upload->setText("");
    }
    if (path == "")
    {
        path = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
    }

    QString dir = QFileDialog::getOpenFileName (this, tr("选择上传文件"),
        path);

    if(dir != "")
    {
     fileDir = dir;
     QByteArray qByteArray;
     qByteArray = dir.toLatin1();
     ptr = strrchr(qByteArray.data(),'/');
     str = QString(QLatin1String(ptr+1));
     ui->label_file->setText(str);
     //选择文件后，enable上传按钮
     ui->pushButton_upload->setEnabled(true);
    }
}

void UpdateDialog::on_pushButton_upload_clicked()
{
    QString fileName;


    fileName = ui->label_file->text();
    //判断是否获取到升级包文件名
    if(fileName == ""){
       QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请选择一个升级包"),QMessageBox::Ok);
        return;
    }

    QString ip = ui->lineEdit_IP->text();
   //判断是否获取到模块IP地址
    if(ip == ""){
         QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址不能为空"),QMessageBox::Ok);
         return ;
    }
    QString pattern("^([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5]).([0-9]{1,2}|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");//匹配的模式
    QRegExp rx(pattern);
    if(!rx.exactMatch(ip)) {//要匹配的输入字符串
       QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址格式不正确！"));
       return ;
    }
    ui->pushButton_browser->setDisabled(true);
    ui->pushButton_upload->setDisabled(true);
    ui->listView->setDisabled(true);
        ftp = new QFtp(this);
        ftp->connectToHost(ip,21); //连接到服务器
        ftp->login("ftpuser","ftp@MHSG");   //登录

        //从配置文件里获取参数
        QString aDir = QCoreApplication::applicationDirPath();
        aDir += "/config.ini";
        qWarning(aDir.toStdString().c_str());
        config = new QSettings(aDir,QSettings::IniFormat);
        ip_6281_Module_1 = config->value("/updateSystem/ip_6281_address13").toString().toStdString();
        if(ip_6281_Module_1==""){
            ip_6281_Module_1 = "127.0.0.1";
        }
        ip_6281_Module_2 = config->value("/updateSystem/ip_6281_address14").toString().toStdString();
        if(ip_6281_Module_2==""){
            ip_6281_Module_2 = "127.0.0.1";
        }

        if(0 == ip.compare(QString(QString::fromLocal8Bit(ip_6281_Module_1.c_str()))) || 0 == ip.compare(QString(QString::fromLocal8Bit(ip_6281_Module_2.c_str())))){

        }else{
            ftp->cd("/hda1/tmp");
        }

    //文件上传

        QString des = ui->label->text();

        if(fileDir != ""){
            des = fileDir;
        }
        QFile *uploadFile = new QFile(fileDir);
        uploadFile->open(QIODevice::ReadWrite);
        ftp->put(uploadFile,fileName);

        connect(ftp,SIGNAL(commandStarted(int)),this,SLOT(ftpCommandStarted(int)));

        //当每条命令开始执行时发出相应的信号
        connect(ftp,SIGNAL(commandFinished(int,bool)),this,SLOT(ftpCommandFinished(int,bool)));
}
void UpdateDialog::ftpCommandStarted(int)
{
    if(ftp->currentCommand() == QFtp::ConnectToHost){
        ui->label_upload->setText(tr("正在连接到服务器…"));
    }
    if (ftp->currentCommand() == QFtp::Login){
        ui->label_upload->setText(tr("正在登录…"));
    }
    if (ftp->currentCommand() == QFtp::Cd){
        ui->label_upload->setText(tr("正在切换目录..."));
    }
    if (ftp->currentCommand() == QFtp::Put){
        ui->label_upload->setText(tr("正在上传…"));
    }
    else if (ftp->currentCommand() == QFtp::Close){
        ui->label_upload->setText(tr("正在关闭连接…"));
    }
}


void UpdateDialog::ftpCommandFinished(int,bool error)
{
    if(ftp->currentCommand() == QFtp::ConnectToHost){
        if(error){
            ui->label_upload->setText(tr("连接服务器出现错误:%1").arg(ftp->errorString() ));
            ui->pushButton_browser->setDisabled(false);
            ui->pushButton_upload->setDisabled(false);
            ui->listView->setDisabled(false);
        }
        else{
            ui->label_upload->setText(tr("连接到服务器成功"));
        }
    }
    if (ftp->currentCommand() == QFtp::Login){
        if(error){
            ui->label_upload->setText(tr("登录出现错误:%1").arg(ftp->errorString()));
            ui->pushButton_browser->setDisabled(false);
            ui->pushButton_upload->setDisabled(false);
             ui->listView->setDisabled(false);
        }
        else
        {
            ui->label_upload->setText(tr("登录成功"));
        }
    }
    if (ftp->currentCommand() == QFtp::Cd){
        if(error){
            ui->label_upload->setText(tr("切换目录出现错误:%1").arg(ftp->errorString()));
            ui->pushButton_browser->setDisabled(false);
            ui->pushButton_upload->setDisabled(false);
             ui->listView->setDisabled(false);
        }else{
            ui->label_upload->setText(tr("切换目录成功"));
        }
    }
    if (ftp->currentCommand() == QFtp::Put){
        if(error)
        {
            ui->label_upload->setText(tr("上传出现错误:%1").arg(ftp->errorString()));
            ui->pushButton_browser->setDisabled(false);
            ui->pushButton_upload->setDisabled(false);
             ui->listView->setDisabled(false);
        }
        else {
            ui->label_upload->setText(tr("上传成功"));
            //开始升级
            updateSys();
            //ui->pushButton_update->setEnabled(true);
        }
    }
    if (ftp->currentCommand() == QFtp::Get){
        if(error)
        {
            ui->label_upload->setText(tr("下载出现错误:%1").arg(ftp->errorString()));
            ui->pushButton_browser->setDisabled(false);
            ui->pushButton_upload->setDisabled(false);
             ui->listView->setDisabled(false);
        }
        else {
            ui->label_upload->setText(tr("已经完成下载"));
        }
    }
    else if (ftp->currentCommand() == QFtp::Close){
       // ui->label_upload->setText(tr("已经关闭连接"));
    }
}

void UpdateDialog::updateSys()
{

    ErrorInfo e;
    QString fileName,ip;

    QString updateUrl = "";
    fileName = ui->label_file->text();

    if(fileName == ""){
        return;
    }

    ip = ui->lineEdit_IP->text();
    if(ip == ""){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("IP地址不能为空"),QMessageBox::Ok);
        return;
    }
    if(0 == ip.compare(QString(QString::fromLocal8Bit(ip_6281_Module_1.c_str())))||0== ip.compare(QString(QString::fromLocal8Bit(ip_6281_Module_2.c_str())))){
       updateUrl = "http://" + ui->lineEdit_IP->text() +"/soapserver.php";
    }else{
       updateUrl = "http://" + ui->lineEdit_IP->text() +":5536";
    }

    mcrClient->update(&fileName,updateUrl.toLatin1().data(),e);

    //升级6281
   if(0 == ip.compare(QString(QString::fromLocal8Bit(ip_6281_Module_1.c_str())))|| 0 == ip.compare(QString(QString::fromLocal8Bit(ip_6281_Module_2.c_str())))) {
    if(e.result == 0){
      QMessageBox::information(this,QObject::tr("提示"),QObject::tr("升级完成"),QMessageBox::Ok);
      ui->label_upload->setText(tr("升级成功"));
       ui->listView->setDisabled(false);
      // this->close();
    }else{
      QMessageBox::information(this,QObject::tr("提示"),QObject::tr("升级失败"),QMessageBox::Ok);
      ui->pushButton_browser->setDisabled(false);
      ui->pushButton_upload->setDisabled(false);
      ui->listView->setDisabled(false);
      ui->label_upload->setText(QObject::tr("升级失败"));
    }
    return;
  }

   //升级6467
    int Status = 0;
    int mid = 0,cid = 0;

    int result_count = 0;
    while(Status  != 2){
        mcrClient->checkUpdateStatus(Status,mid,cid,updateUrl.toLatin1().data(),e);
        if(e.result == -256){
            if(result_count == 3){
               // ui->label_upload->setText(tr("升级失败"));
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("升级失败"),QMessageBox::Ok);
                ui->pushButton_browser->setDisabled(false);
                ui->pushButton_upload->setDisabled(false);
                ui->listView->setDisabled(false);
                 ui->label_upload->setText(tr("升级失败"));
                break;
            }else{
                result_count ++;
                continue;
            }
        }else{
            if(Status < 0){
             //   ui->label_upload->setText(tr("升级失败"));
                QMessageBox::information(this,QObject::tr("提示"),QObject::tr("升级失败"),QMessageBox::Ok);
                ui->pushButton_browser->setDisabled(false);
                ui->pushButton_upload->setDisabled(false);
                ui->listView->setDisabled(false);
                ui->label_upload->setText(tr("升级失败"));
                break;
            }else if(Status == 1){
                sleep(1);
                continue;
            }
        }
    }
    if(Status == 2){
       // ui->label_upload->setText(tr("升级成功"));
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("升级成功"),QMessageBox::Ok);
        ui->label_upload->setText(tr("升级成功"));
        ui->listView->setDisabled(false);
        // this->close();
    }
}

void UpdateDialog::on_listView_pressed(const QModelIndex &index)
{
    if("" != ui->lineEdit_IP->text()){
        ui->lineEdit_IP->setText("");
    }
    if("" != ui->label_upload->text()){
        ui->label_upload->setText("");
    }
    if("" != ui->label_file->text()){
        ui->label_file->setText("");
    }
    if(ui->pushButton_upload->isEnabled()){
        ui->pushButton_upload->setDisabled(true);
    }
    if(!ui->pushButton_browser->isEnabled()){
        ui->pushButton_browser->setEnabled(true);
    }
}
