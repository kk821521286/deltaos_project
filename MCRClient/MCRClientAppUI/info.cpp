#include "info.h"
#include "ui_info.h"
Info::Info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Info)
{
    ui->setupUi(this);
    Qt::WindowFlags flag;
    flag = windowFlags();

    flag = flag & (~Qt::WindowContextHelpButtonHint);

    setWindowFlags(flag);
}

