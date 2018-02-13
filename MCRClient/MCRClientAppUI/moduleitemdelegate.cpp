#include "moduleitemdelegate.h"
#include <QPainter>
#include <QApplication>
#include <QStandardItem>
#include <QVariant>


ModuleItemDelegate::ModuleItemDelegate(QTreeView* view)
{
    treeView = view;
}

ModuleItemDelegate::~ModuleItemDelegate()
{

}


void ModuleItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const{
    if(!index.isValid()){
        return;
    }
    QStandardItemModel* model = static_cast<QStandardItemModel*>(treeView->model());
    QStandardItem* item = model->itemFromIndex(index);
    QVariant v = index.data(Qt::UserRole+1);
    if(v.isNull()){
        qWarning("no data:index:%d,%d\n",index.row(),index.column());

    }
    QStyledItemDelegate::paint(painter, option, index);
    if(v.canConvert<ModuleInfoData>()){
        ModuleInfoData moduleInfoData = v.value<ModuleInfoData>();
        if(index.column()==0){
            item->setText(moduleInfoData.moduleConfig.name.c_str());
            //painter->drawText(option.rect,Qt::AlignLeft|Qt::AlignVCenter,moduleInfoData.moduleConfig.name.c_str());
        }else if(index.column() == 1) {
            item->setIcon(QIcon(":/image/config.png"));
        }
        //painter->drawText(option.rect,Qt::AlignLeft|Qt::AlignVCenter,moduleInfoData.moduleConfig.name.c_str());
    }else if(v.canConvert<ChannelInfoData>()){
        ChannelInfoData channelInfoData = v.value<ChannelInfoData>();
        if(index.column()==0){
            item->setIcon(QIcon(":/image/playing&norecording.png"));
            //QPixmap recordPixmap(":/image/playing&norecording.png");
            //painter->drawPixmap(option.rect,recordPixmap,recordPixmap.rect());
        }else if(index.column() == 1){
            item->setText(channelInfoData.channelConfig.streamUrl.c_str());
            //painter->drawText(option.rect,Qt::AlignLeft|Qt::AlignVCenter,channelInfoData.channelConfig.streamUrl.c_str());
        }else if(index.column() == 2){
            item->setIcon(QIcon(":/image/config.png"));
            //QPixmap recordPixmap(":/image/config.png");
            //painter->drawPixmap(option.rect,recordPixmap,recordPixmap.rect());

        }


        //QApplication::style()->drawControl();
    }

    return;
}
bool ModuleItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index){

    return QStyledItemDelegate::editorEvent(event,model,option,index);
}
