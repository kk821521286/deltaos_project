#include "fileitemdelegate.h"
#include <QPainter>
#include <QApplication>
#include <QStandardItem>
#include <QVariant>
FileItemDelegate::FileItemDelegate()
{

}
void FileItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    if(!index.isValid()){
        return;
    }
    QModelIndex idx = index.model()->index(index.row(),0);
    QStyledItemDelegate::paint(painter, option, index);
    QVariant v = idx.data(Qt::UserRole+1);
    if(v.canConvert<FileInfoData>()){
        /*
        if(index.column()==5){
            FileInfoData fid = v.value<FileInfoData>();
            std::string displayContent="";
            if(fid.downloading){
                displayContent.append("下载中");
                int percent = fid.downloadsize * 100 / fid.fi.size ;
                char tmp[32];
                sprintf(tmp,"%d\%",percent);
                displayContent.append(tmp);
            }else{
                if(fid.downloadsize == fid.fi.size){
                    displayContent.append("已下载");
                }
                if(fid.downloadsize == 0){
                    displayContent.append("未下载");
                }

            }
            painter->drawText(option.rect,QObject::tr(displayContent.c_str()));


        }*/

    }


    return;
}

