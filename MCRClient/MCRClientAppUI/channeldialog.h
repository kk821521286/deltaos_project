#ifndef CHANNELDIALOG_H
#define CHANNELDIALOG_H

#include <QDialog>
#include"mcrclient.h"
#include <QStandardItemModel>
#include "mcrclient.h"
#include<QThread>
#include <QSettings>
#include "dialog.h"
#include<QLabel>
namespace Ui {
class ChannelDialog;


}
class CheckConfInfoListThread;
class ChannelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelDialog(QWidget *parent = 0);

    ~ChannelDialog();
    CheckConfInfoListThread* checkConfInfoListThread;
    QLabel* alertLabel;
    QDialog* alertDlg;
    QSettings* config;
    ChannelDialog *channelDialog;


    void setChannelConfig(ChannelConfig &cConf,ChannelDialog *cDialog);
    void showChannelConfig();
    void showStreamConfig();
    void showRecordConfig();
    void showConfigInfo();
    void showChannelInterface();
    void showStreamInterface();
    void showRecordInterface();
private slots:
    void on_btnChannel_clicked();

    void on_btnStream_clicked();

    void on_btnRecord_clicked();

    void on_btnSaveAll_clicked();

private:
    Ui::ChannelDialog *ui;

    ChannelConfig v_cConf;


    QStandardItemModel* modelTablecConf;
    QStandardItemModel* modelTablesConf;
    QStandardItemModel* modelTablerConf;
    MCRClient *mcrClient;
     StreamConfig sConf;
     ChannelConfig cConf;
     RecordConfig rConf;
    std::string moduleServerUrl;



};
class CheckConfInfoListThread: public QThread{
public :
    CheckConfInfoListThread(ChannelDialog* cdlg,QThread* in = NULL);
protected :
    void run();
private:
    ChannelDialog* cdlg;
};




#endif // CHANNELDIALOG_H
