#ifndef DOWNLOAD_H
#define DOWNLOAD_H


#include<QNetworkReply>
#include<QNetworkAccessManager>
#include<QFile>
#include<QUrl>
#include<QMutex>
class Download : public QObject
{
    Q_OBJECT
public:
    Download();
    ~Download();
   // bool getFileFromURL(const QUrl &url, QString &filePath);


    void getFileFromURL(const QUrl &url, QString &filePath, qint64 size);
   // void getFileFromURLMulti(const QUrl &url, QString &filePath,qint64 startPoint,qint64 endPoint);
    void SetErrorMessage(const QString &msg);
    void stopDownload();
    qint64 getProgress();

    qint64 getFileSize(QUrl url);
    bool isFinished();


private:

    QFile *file;
    QString _errMsg;
    QNetworkReply *reply;
    QNetworkAccessManager qnam;
    qint64 downsize, readSize, totalSize;
    QMutex mutex;
    bool finished;
private slots:
    void httpReadyRead();
    void httpFinished();
    void updateDataReadProgress(qint64,qint64);

};

#endif // DOWNLOAD_H
