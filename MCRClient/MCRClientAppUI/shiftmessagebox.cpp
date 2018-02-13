#include "shiftmessagebox.h"
#include "ui_shiftmessagebox.h"

static ShiftMessageBox* _getInstance = NULL;

ShiftMessageBox::ShiftMessageBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShiftMessageBox)
{
    ui->setupUi(this);
    result = 0;
}

ShiftMessageBox::~ShiftMessageBox()
{
    delete ui;
}
ShiftMessageBox* ShiftMessageBox::getInstance()
{
    if(_getInstance == NULL){
        _getInstance = new ShiftMessageBox();
        return _getInstance;
    }
    return _getInstance;
}
int  ShiftMessageBox::getResult()
{
    return result;
}
void ShiftMessageBox::resetResult()
{
    result = 0;
}
void ShiftMessageBox::on_pBt_HKB_clicked()
{
    result = 1;
    close();
}

void ShiftMessageBox::on_pBt_BD_clicked()
{
    result = 2;
    close();
}
