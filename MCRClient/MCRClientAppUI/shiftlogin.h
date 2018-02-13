#ifndef SHIFTLOGIN_H
#define SHIFTLOGIN_H

#include <QDialog>

namespace Ui {
class ShiftLogin;
}

class ShiftLogin : public QDialog
{
    Q_OBJECT

public:
    explicit ShiftLogin(QWidget *parent = 0);
    ~ShiftLogin();
    static ShiftLogin* getInstance();
    int getResult();
    void clearPassword();

private slots:
    void on_pBt_commit_clicked();

    void on_pBt_cancel_clicked();

private:
    Ui::ShiftLogin *ui;
    int _result;
};

#endif // SHIFTLOGIN_H
