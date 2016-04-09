#ifndef __MUTEX_H
#define __MUTEX_H

#include <cstdlib>  
#include <pthread.h>  
#include <errno.h>
#include <unistd.h>
#include <cstdio>
#include <string.h>
#include <sys/time.h>

class Mutex  
{  
public:  
    Mutex();  
    virtual ~Mutex();  
  
    void Lock();  
    void UnLock();  
private:  
friend class CondVar;  
    pthread_mutex_t m_mutex;  
    Mutex(const Mutex&);  
    void operator=(const Mutex&);  
};  
  
class CondVar  
{  
public:  
    explicit CondVar(Mutex* mu);  
    virtual ~CondVar();  
      
    void Wait(int milliseconds = 0);  
    void Signal();  
    void SignalAll();  
private:  
    pthread_cond_t m_cond;  
    Mutex *m_mu;  
};  

class CASLock
{
public:
    CASLock(void* lock);
    virtual ~CASLock();
    void Lock();
	void UnLock();
	bool TryLock();
private:
	void* m_lock;
};

static void MutexErr(const char* err, int res)  
{  
    if(res != 0)  
    {  
        fprintf(stderr, "pthread %s: %s\n", err, strerror(res));  
        return;  
    }  
}  
  

#endif //__MUTEX_H
