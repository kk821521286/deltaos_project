#ifndef SHIFTMESSAGEBOX_H
#define SHIFTMESSAGEBOX_H

#include <QDialog>

namespace Ui {
class ShiftMessageBox;
}

class ShiftMessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit ShiftMessageBox(QWidget *parent = 0);
    ~ShiftMessageBox();
    void resetResult();
    static ShiftMessageBox *getInstance();
    int getResult();

private slots:
    void on_pBt_HKB_clicked();

    void on_pBt_BD_clicked();

private:
    Ui::ShiftMessageBox *ui;

    int result;
};

#endif // SHIFTMESSAGEBOX_H
