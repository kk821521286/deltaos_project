#include "shiftlogin.h"
#include "ui_shiftlogin.h"
#include <QMessageBox>
static ShiftLogin* _shiftLogin = NULL;
ShiftLogin::ShiftLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShiftLogin)
{
    ui->setupUi(this);
    ui->lineEdit_secret->setEchoMode(QLineEdit::Password);
    _result = 0;
}

ShiftLogin::~ShiftLogin()
{
    delete ui;
}
ShiftLogin* ShiftLogin::getInstance()
{
    if(_shiftLogin == NULL){
        _shiftLogin = new ShiftLogin();
        return _shiftLogin ;
    }
    return _shiftLogin;
}
void ShiftLogin::on_pBt_commit_clicked()
{
    QString userName = ui->lineEdit_userName->text();
    QString passwd = ui->lineEdit_secret->text();

    if(userName == ""){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("用户名不能为空！"),QMessageBox::Ok);
        return;
    }else if(0 == userName.compare("admin")){

    }else{
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请输入正确用户名！"),QMessageBox::Ok);
        _result = 2;
        return;
    }



    if(passwd == ""){
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("密码不能为空！"),QMessageBox::Ok);
        return;
    }else if(0 == passwd.compare("admin")){
        _result = 1;
        close();
    }else {
        QMessageBox::information(this,QObject::tr("提示"),QObject::tr("请输入正确密码！"),QMessageBox::Ok);
        _result = 2;
    }
}
void ShiftLogin::clearPassword()
{
    _result = 0;
    ui->lineEdit_secret->setText("");
    ui->lineEdit_userName->setText("");
}
int ShiftLogin::getResult()
{
    return _result;
}

void ShiftLogin::on_pBt_cancel_clicked()
{
       close();
}
