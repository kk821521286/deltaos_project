#ifndef UPDATEIPDELEGATE_H
#define UPDATEIPDELEGATE_H

#include<QItemDelegate>
#include <QSettings>
class UpdateIpDelegate :public QItemDelegate
{
public:
    UpdateIpDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor,const QModelIndex &index)const;
    void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index)const;
    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,const QModelIndex &index)const;
    QSettings *config;
    std::string updateIP[15];
};

#endif // UPDATEIPDELEGATE_H
