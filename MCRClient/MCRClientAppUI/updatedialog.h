#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QFtp>
#include "dialog.h"
#include "mcrclient.h"
namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = 0);
    ~UpdateDialog();
    void updateSys();//升级函数
private slots:
    void on_pushButton_browser_clicked();//选择上传文件函数

    void on_pushButton_upload_clicked();//上传文件函数

    void ftpCommandStarted(int);
    void ftpCommandFinished(int,bool);

    void on_listView_pressed(const QModelIndex &index);

private:
    Ui::UpdateDialog *ui;
    QFtp *ftp;
    QStandardItemModel* modelTableIpConf;
    QString fileDir;
    MCRClient *mcrClient;//依赖库的指针变量
    QSettings* config;//获取配置文件参数的指针变量
    QString str;
    std::string moduleServerUrl;//模块服务器地址
    std::string ip_6281_Module_1;
    std::string ip_6281_Module_2;

};

#endif // UPDATEDIALOG_H
