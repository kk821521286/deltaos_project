#ifndef ADDMODULEDIALOG_H
#define ADDMODULEDIALOG_H

#include <QDialog>
#include"mcrclient.h"
#include <QStandardItemModel>

#include<QThread>
#include <QSettings>
#include "dialog.h"
#include<QLabel>
namespace Ui {
class AddModuleDialog;
}
class CheckAddModuleConfInfoThread;
class AddModuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddModuleDialog(QWidget *parent = 0);

    ~AddModuleDialog();
    CheckAddModuleConfInfoThread* checkAddModuleConfInfoThread;
    QLabel* alertLabel;
    QDialog* alertDlg;
    QSettings* config;
    AddModuleDialog *addModuleDialog;
    int flag;
    void showModuleConfig();
    void setModuleConfig(AddModuleDialog *addMDialog);

    void showConfigInfo();
    int addMid();
    void showModuleInterface();

private slots:
    void on_btn_AddModule_clicked();

private:
    Ui::AddModuleDialog *ui;

    ModuleConfig v_mConf;


    QStandardItemModel* modelTablemConf;

    MCRClient *mcrClient;
     ModuleConfig mConf;

    std::string moduleServerUrl;



};
class CheckAddModuleConfInfoThread: public QThread{
public :
    CheckAddModuleConfInfoThread(AddModuleDialog* amdlg,QThread* in = NULL);
protected :
    void run();
private:
    AddModuleDialog* amdlg;
};




#endif // ADDMODULEDIALOG_H
