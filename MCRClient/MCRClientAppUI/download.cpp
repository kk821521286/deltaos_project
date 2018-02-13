#include "download.h"

#include<QDebug>
#include<QFileInfo>
#include <QCoreApplication>
Download::Download()
{
    finished = true;
}
Download::~Download()
{

}



void Download::SetErrorMessage(const QString &msg)

{

qDebug()<<(msg);

_errMsg = msg;

}
void Download::stopDownload()
{
    if(reply){
        reply->abort();
        finished = true;
    }
    printf("you stop the download\n");
}


void Download::httpReadyRead()
{
    mutex.lock();
    if(file)
        file->write(reply->readAll());
    mutex.unlock();
    //qDebug("httpReadyRead function call");
}

void Download::httpFinished()
{
    mutex.lock();
    if(file)
        file->write(reply->readAll());
    mutex.unlock();
    file->flush();
    file->close();
    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;
    finished = true;
    qWarning("file receive success\n");
}
void Download::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{

    //qDebug()<<QString("------->%1%").arg((downsize + bytesRead) / (double)totalSize * 100);
    readSize = bytesRead;

}
void Download::getFileFromURL(const QUrl &url, QString &filePath, qint64 tsize)
{
    finished = false;
    totalSize = tsize;
    QNetworkRequest qheader;



    if (!url.isValid())
    {
        SetErrorMessage(QString("Error:URL has specify a invalid name."));
        return ;
    }

    if (url.scheme() != "http")
    {
        SetErrorMessage(QString("Error:URL must start with 'http:'"));
        return ;
    }


    if (url.path().isEmpty())
    {
        SetErrorMessage(QString("Error:URL's path is empty."));
        return ;
    }


    if (filePath.isEmpty())
    {
        SetErrorMessage(QString("Error:invalid filePath."));
        return ;
    }
    file = new QFile(filePath);

    if (!file->open(QIODevice::Append))
    {
        SetErrorMessage(QString("Error:Cannot write file."));
        return ;
    }
    downsize = file->size();
    qheader.setUrl(url);
    qheader.setRawHeader("Range",tr("bytes=%1-").arg(downsize).toUtf8());
    reply = qnam.get(QNetworkRequest(qheader));


    connect(reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()),Qt::QueuedConnection);
    connect(reply,SIGNAL(finished()),this,SLOT(httpFinished()));
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateDataReadProgress(qint64,qint64)));
}

qint64 Download::getFileSize(QUrl url)
{
    QNetworkAccessManager manager;
    qDebug() << "Getting the file size...";
    QEventLoop loop;
    //发出请求，获取目标地址的头部信息
    QNetworkReply *reply = manager.head(QNetworkRequest(url));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()), Qt::DirectConnection);
    loop.exec();
    QVariant var = reply->header(QNetworkRequest::ContentLengthHeader);
    delete reply;
     totalSize = var.toLongLong();
    qDebug() << "The file size is: " << totalSize;
    return totalSize;
}
// void Download::getFileFromURLMulti(const QUrl &url, QString &filePath,qint64 startPoint,qint64 endPoint)
// {
//     QNetworkRequest qheader;



//     if (!url.isValid())
//     {
//         SetErrorMessage(QString("Error:URL has specify a invalid name."));
//         return ;
//     }

//     if (url.scheme() != "http")
//     {
//         SetErrorMessage(QString("Error:URL must start with 'http:'"));
//         return ;
//     }


//     if (url.path().isEmpty())
//     {
//         SetErrorMessage(QString("Error:URL's path is empty."));
//         return ;
//     }


//     if (filePath.isEmpty())
//     {
//         SetErrorMessage(QString("Error:invalid filePath."));
//         return ;
//     }
//     file = new QFile(filePath);

//     if (!file->open(QIODevice::Append))
//     {
//         SetErrorMessage(QString("Error:Cannot write file."));
//         return ;
//     }
//     downsize = file->size();
//     qheader.setUrl(url);
//     qheader.setRawHeader("Range",tr("bytes=%1-").arg(downsize).toUtf8());
//     reply = qnam.get(QNetworkRequest(qheader));


//     connect(reply,SIGNAL(readyRead()),this,SLOT(HttpReadyRead()),Qt::QueuedConnection);
//     connect(reply,SIGNAL(finished()),this,SLOT(HttpFinished()));
//     connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(UpdateDataReadProgress(qint64,qint64)));

// }

 qint64 Download::getProgress(){
     return (this->downsize + this->readSize);

 }
 bool Download::isFinished(){
     return finished;
 }
