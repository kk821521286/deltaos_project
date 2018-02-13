#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include <QThread>
#include <QRunnable>
typedef void (*WorkFunc)(void* ctx);

class WorkerThread : public QThread
{
public:
    WorkerThread(void* ctx,WorkFunc func, QThread* in = NULL);

protected :
    void run();
private:
    void* ctx;
    WorkFunc func;

};

class WorkerPoolThread : public QRunnable
{
public:
    WorkerPoolThread(void* ctx,WorkFunc func);
    ~WorkerPoolThread();

protected :
    void run();
private:
    void* ctx;
    WorkFunc func;
    void* param;

};

#endif // WORKERTHREAD_H
