#include "workerthread.h"

WorkerThread::WorkerThread(void* ctx, WorkFunc func, QThread* in):
    QThread(in)
{
    this->ctx = ctx;
    this->func = func;
}
//WorkerThread::~WorkerThread()
//{

//}
void WorkerThread::run(){
    func(ctx);
}

