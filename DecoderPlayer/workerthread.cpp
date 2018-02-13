#include "workerthread.h"

WorkerThread::WorkerThread(void* ctx, WorkFunc func, QThread* in):
    QThread(in)
{
    this->ctx = ctx;
    this->func = func;
}

void WorkerThread::run(){
    func(ctx);
}

WorkerPoolThread::WorkerPoolThread(void* ctx,WorkFunc func)
{
    this->ctx = ctx;
    this->func = func;
}
WorkerPoolThread::~WorkerPoolThread()
{

}

void WorkerPoolThread::run()
{
    func(ctx);
}
