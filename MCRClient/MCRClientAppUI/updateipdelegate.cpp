#include "updateipdelegate.h"
#include <QPainter>
#include<QComboBox>
#include <QApplication>
#include <QStandardItem>
#include <QVariant>

UpdateIpDelegate::UpdateIpDelegate(QObject *parent):
        QItemDelegate(parent)
{
    QString aDir = QCoreApplication::applicationDirPath();
    aDir += "/config.ini";
    qWarning(aDir.toStdString().c_str());
    config = new QSettings(aDir,QSettings::IniFormat);

    int i;
    for(i=1;i<=12;i++){
        updateIP[i] = config->value("/updateSystem/ip_6467_address"+QString::number(i)).toString().toStdString();
        if(updateIP[i]==""){
            updateIP[i] = "127.0.0.1";
        }
    }
    for(i=13;i<=14;i++){
        updateIP[i] = config->value("/updateSystem/ip_6281_address"+QString::number(i)).toString().toStdString();
        if(updateIP[i]==""){
            updateIP[i] = "127.0.0.1";
        }
    }
}
QWidget *UpdateIpDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);


    editor->addItem(QString(QString::fromLocal8Bit(updateIP[1].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[2].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[3].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[4].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[5].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[6].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[7].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[8].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[9].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[10].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[11].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[12].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[13].c_str())));
    editor->addItem(QString(QString::fromLocal8Bit(updateIP[14].c_str())));
    editor->installEventFilter(const_cast<UpdateIpDelegate*>(this));
    return editor;
}

void UpdateIpDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString str = index.model()->data(index).toString();
    QComboBox *box = static_cast<QComboBox*>(editor);
    int i = box->findText(str);
    box->setCurrentIndex(i);
}

void UpdateIpDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *box = static_cast<QComboBox*>(editor);
    QString str = box->currentText();

    model->setData(index,str);
}

void UpdateIpDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
