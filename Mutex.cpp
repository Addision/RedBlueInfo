#include "Mutex.h"

Mutex::Mutex()  
{  
    MutexErr("init mutex", pthread_mutex_init(&m_mutex, NULL));  
}  
  
Mutex::~Mutex()  
{  
    MutexErr("destroy mutex", pthread_mutex_destroy(&m_mutex));  
}  
  
void Mutex::Lock()  
{  
    MutexErr("lock mutex", pthread_mutex_lock(&m_mutex));  
}  
  
void Mutex::UnLock()  
{  
    MutexErr("unlock mutex", pthread_mutex_unlock(&m_mutex));  
}  
  
CondVar::CondVar(Mutex* mu):m_mu(mu)  
{  
    MutexErr("init cond", pthread_cond_init(&m_cond, NULL));  
}  
  
CondVar::~CondVar()  
{  
    MutexErr("destroy cond", pthread_cond_destroy(&m_cond));  
}  
  
void CondVar::Wait(int milliseconds)  
{
	if(milliseconds == 0)
	{
		MutexErr("wait cond", pthread_cond_wait(&m_cond, &m_mu->m_mutex));  		
	}
	else{
		struct timeval curtime;

		struct timespec abstime;
		gettimeofday(&curtime, NULL);

		long long us = (static_cast<long long>(curtime.tv_sec) *
						static_cast<long long>(1000000) +
						static_cast<long long>(curtime.tv_usec) +
						static_cast<long long>(milliseconds) *
						static_cast<long long>(1000));

		abstime.tv_sec = static_cast<int>(us / static_cast<long long>(1000000));
		abstime.tv_nsec = static_cast<int>(us % static_cast<long long>(1000000)) * 1000;
		MutexErr("wait time cond", pthread_cond_wait(&m_cond, &m_mu->m_mutex));
	}
}  
  
void CondVar::Signal()  
{  
    MutexErr("signal cond", pthread_cond_signal(&m_cond));  
}  
  
void CondVar::SignalAll()  
{  
    MutexErr("broadcast cond", pthread_cond_broadcast(&m_cond));  
}

CASLock::CASLock(void* lock):m_lock(lock){}
CASLock::~CASLock(){}

void CASLock::Lock()
{
	while(!__sync_bool_compare_and_swap((int*)m_lock, 0, 1))
	{
		sched_yield();
	}
}

void CASLock::UnLock()
{
	*((int*)m_lock) = 0;
}

bool CASLock::TryLock()
{
	if(__sync_bool_compare_and_swap((int*)m_lock, 0, 1))
		return 0;
	else
		return -1;
}
