#ifndef MODULEDIALOG_H
#define MODULEDIALOG_H

#include <QDialog>
#include"mcrclient.h"
#include <QStandardItemModel>

#include<QThread>
#include <QSettings>
#include "dialog.h"
#include<QLabel>
namespace Ui {
class ModuleDialog;


}
class CheckModuleConfInfoThread;
class ModuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModuleDialog(QWidget *parent = 0);

    ~ModuleDialog();
    CheckModuleConfInfoThread* checkModuleConfInfoThread;
    QLabel* alertLabel;
    QDialog* alertDlg;
    QSettings* config;
    ModuleDialog *moduleDialog;

    void setModuleConfig(ModuleConfig &mConf,ModuleDialog *mDialog);
    void showConfigInfo();
    void showModuleConfig();
    void showModuleInterface();

private slots:
    void on_btnModule_clicked();


    void on_btn_RebootModule_clicked();


private:
    Ui::ModuleDialog *ui;

    ModuleConfig v_mConf;


    QStandardItemModel* modelTablemConf;

    MCRClient *mcrClient;
     ModuleConfig mConf;

    std::string moduleServerUrl;



};
class CheckModuleConfInfoThread: public QThread{
public :
    CheckModuleConfInfoThread(ModuleDialog* mdlg,QThread* in = NULL);
protected :
    void run();
private:
    ModuleDialog* mdlg;
};




#endif // MODULEDIALOG_H
