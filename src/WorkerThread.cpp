#include "../include/WorkerThread.h"
WorkerThread::WorkerThread(int index)
{
	m_evLoop = nullptr;
	m_thread = nullptr;
	m_threadID = thread::id();
	m_name = "SubThread-" + to_string(index);
}

WorkerThread::~WorkerThread()
{
	// 子线程一直在线程池中运行，不会析构
	if (m_thread != nullptr)
	{
		delete m_thread;
	}
}

void WorkerThread::Run()
{
	//创建子线程,3,4子线程的回调函数以及传入的参数
	//调用的函数，以及此函数的所有者this
	m_thread = new thread(&WorkerThread::Running,this);
	// 阻塞主线程，让当前函数不会直接结束，不知道当前子线程是否运行结束
	// 如果为空，子线程还没有初始化完毕,让主线程等一会，等到初始化完毕
	unique_lock<mutex> locker(m_mutex);
	while (m_evLoop == nullptr)
	{
		m_cond.wait(locker);
	}
}

void* WorkerThread::Running()
{
	m_mutex.lock();
	//对evLoop做初始化
	m_evLoop = new EventLoop(m_name);
	m_mutex.unlock();
	m_cond.notify_one(); //唤醒一个主线程的条件变量等待解除阻塞
	// 启动反应堆模型
	m_evLoop->Run();
	return this;
}
