#ifndef INFO_H
#define INFO_H

#include <QMainWindow>
#include <QObject>
#include <QDialog>
#include <QWidget>

namespace Ui {
class Info;
}
class Info: public QDialog
{
     Q_OBJECT
public:
    explicit Info(QWidget *parent = 0);
private:
    Ui::Info *ui;
};

#endif // INFO_H
