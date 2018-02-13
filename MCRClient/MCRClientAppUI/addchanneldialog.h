#ifndef ADDCHANNELDIALOG_H
#define ADDCHANNELDIALOG_H

#include <QDialog>
#include"mcrclient.h"
#include <QStandardItemModel>

#include<QThread>
#include <QSettings>
#include "dialog.h"
#include<QLabel>
namespace Ui {
class AddChannelDialog;


}
class CheckAddChannelConfInfoThread;
class AddChannelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddChannelDialog(QWidget *parent = 0);

    ~AddChannelDialog();
    CheckAddChannelConfInfoThread* checkAddChannelConfInfoThread;
    QLabel* alertLabel;
    QDialog* alertDlg;
    QSettings* config;
    AddChannelDialog *addChannelDlg;
    int flag;


    void setChannelConfig(std::string &serverUrl,ChannelInfoData &cInfoData,AddChannelDialog *addCDialog);
    void showConfigInfo();
    void showChannelConfig();
    void showChannelRecordInterface();
    void showChannelRecordConfig();
    void showChannelInterface();

private slots:
    void on_btn__AddChannel_clicked();

private:
    Ui::AddChannelDialog *ui;

    ChannelInfoData v_cInfoData;


    QStandardItemModel* modelTablecConf;
    QStandardItemModel* modelTablerConf;
    MCRClient *mcrClient;
     ChannelConfig cConf;

    std::string moduleServerUrl;



};
class CheckAddChannelConfInfoThread: public QThread{
public :
    CheckAddChannelConfInfoThread(AddChannelDialog* acdlg,QThread* in = NULL);
protected :
    void run();
private:
    AddChannelDialog* acdlg;
};




#endif // ADDCHANNELDIALOG_H
