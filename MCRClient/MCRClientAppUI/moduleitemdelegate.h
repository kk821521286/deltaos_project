#ifndef MODULEITEMDELEGATE_H
#define MODULEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QTreeView>
#include <QThread>
#include <QProcess>
#include <QTimer>
#include "workerthread.h"
#include "mcrclient.h"

typedef struct ModuleInfoData{
    ModuleConfig moduleConfig;
}ModuleInfoData;
Q_DECLARE_METATYPE(ModuleInfoData);

typedef struct ChannelInfoData{
    ChannelConfig channelConfig;
    int streamStatus;
    int recordStatus;
    int streamOnline;
    int recordOnline;
    int status;
    int isDelete;
    int flag;//定时器更新标签
    WorkerThread* checkStatusThread;
    QProcess *pro ;
}ChannelInfoData;
Q_DECLARE_METATYPE(ChannelInfoData);


class ModuleItemDelegate : public QStyledItemDelegate
{
public:
    ModuleItemDelegate(QTreeView* view);
    ~ModuleItemDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    QTreeView* treeView;
};

#endif // MODULEITEMDELEGATE_H
