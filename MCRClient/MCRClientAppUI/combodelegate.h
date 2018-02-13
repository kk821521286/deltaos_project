#ifndef COMBODELEGATE_H
#define COMBODELEGATE_H


#include<QItemDelegate>
#include <mcrclient.h>


class ComboDelegate : public QItemDelegate
{
public:
    ComboDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor,const QModelIndex &index)const;
    void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index)const;
    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,const QModelIndex &index)const;

};

#endif // COMBODELEGATE_H
