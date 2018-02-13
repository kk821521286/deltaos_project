#ifndef FILEITEMDELEGATE_H
#define FILEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <mcrclient.h>


typedef struct FileInfoData{
    FileInfo fi;
    int mid;
    int cid;
    bool isDelete  ;
    bool downloading  ;
    qint64 downloadsize;
}FileInfoData;
Q_DECLARE_METATYPE(FileInfoData);



class FileItemDelegate : public QStyledItemDelegate
{
public:
    FileItemDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

#endif // FILEITEMDELEGATE_H
