#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include <QThread>
typedef void (*WorkFunc)(void* ctx);

class WorkerThread : public QThread
{
public:
    WorkerThread(void* ctx,WorkFunc func, QThread* in = NULL);
//    ~WorkerThread();
protected :
    void run();
private:
    void* ctx;
    WorkFunc func;

};

#endif // WORKERTHREAD_H
