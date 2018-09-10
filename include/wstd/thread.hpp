windows/linux��ʵ���̵߳ķ�ʽ����ȫ��һ���ġ����������ȶ���һ���߳��ࡣ

[cpp] view plaincopy��CODE�ϲ鿴����Ƭ�������ҵĴ���Ƭ

#ifndef _OSAPI_THREAD_H  
#define _OSAPI_THREAD_H  
  
/* presented by ������� */  
class OS_Thread  
{  
public:  
    OS_Thread();  
    virtual ~OS_Thread();  
  
    // ����������  
    virtual int Run();  
  
    // �ȴ����ջ���Դ  
    static void Join(OS_Thread* thrd);  
  
    // Sleep����  
    static void Msleep(int ms);  
    static void Sleep(int s);  
  
public:  
    virtual int Routine() = 0;  
  
private:  
    void* m_Priv;  
};  
  
  
#endif  

 

Ҳ����˵����������XXXƽ̨����YYYƽ̨���ܸ������߳�֧�ֵġ������鷳���Ȱ��������ӿ�ʵ��һ���߳���ɡ�����ҵ�������Ҫʹ���̵߳�ʱ�򣬾�ʹ���߳̽ӿڣ���ȫ��ʹ��ƽ̨��صĽӿ��ˡ�

[cpp] view plaincopy��CODE�ϲ鿴����Ƭ�������ҵĴ���Ƭ

#include "../osapi/Thread.h"  
  
  
class MyThread : public OS_Thread  
{  
public:  
    int Create()  
    {  
        m_quitflag = false;  
        run();  
        return 0;  
    }  
      
    void Destroy()  
    {  
        m_quitflag = true;  
        Join(this);  
    }  
  
private:  
    /* �̺߳��� */  
    virtual int Routine()  
    {  
        while(!m_quitflag)  
        {  
            printf("helloafa!\n");  
            OS_Thread::msleep(500);  
        }  
        return 0;  
    }  
      
private:  
    bool m_quitflag;  
};  

��Windows�ϣ�����дһ��ʵ��Thread_Win32.cpp


 

[cpp] view plaincopy��CODE�ϲ鿴����Ƭ�������ҵĴ���Ƭ

#ifdef _WIN32  
  
#include <windows.h>  
#include <process.h>   
  
#include "Thread.h"  
  
struct OS_Thread_Priv  
{  
    HANDLE hThread;  
};  
  
OS_Thread::OS_Thread()   
: m_Priv(NULL)  
{  
}  
  
OS_Thread::~OS_Thread()  
{  
    if(m_Priv)  
    {  
        OS_Thread_Priv* priv = (OS_Thread_Priv*) m_Priv;  
        delete priv;  
    }  
}  
  
static DWORD WINAPI OS_Thread_Proc_Win32_1(LPVOID param)  
{  
    OS_Thread* thrd = (OS_Thread*) param;  
    thrd->Routine();  
  
    return 0;  
}  
  
static void OS_Thread_Proc_Win32_2(void* param)  
{  
    OS_Thread* thrd = (OS_Thread*) param;  
    thrd->Routine();  
}  
  
static unsigned int WINAPI OS_Thread_Proc_Win32_3(void* param)  
{  
    OS_Thread* thrd = (OS_Thread*) param;  
    thrd->Routine();  
    return 0;  
}  
  
int OS_Thread::Run()  
{  
    // ����˽�нṹ  
    OS_Thread_Priv* priv = new OS_Thread_Priv;  
    if(!priv) return -1;  
  
    m_Priv = priv;  
  
    // �����߳�  
//  DWORD nTheadId;  
//  priv->hThread = _beginthreadex(NULL, NULL, OS_Thread_Proc_Win32, this,  0, &nTheadId);  
//  priv->hThread = (HANDLE) _beginthread(OS_Thread_Proc_Win32, 0, this);  
    unsigned int thrdaddr ;  
    priv->hThread = (HANDLE) _beginthreadex(NULL, 0, OS_Thread_Proc_Win32_3, this, 0, &thrdaddr);  
    if(priv->hThread == NULL)  
    {  
        delete priv;  
        m_Priv = NULL;  
        return -1;  
    }  
  
    return 0;  
}  
  
void OS_Thread::Join(OS_Thread* thrd)  
{  
    OS_Thread_Priv* priv = (OS_Thread_Priv*) thrd->m_Priv;  
    if(priv)  
    {  
        WaitForSingleObject(priv->hThread, INFINITE);  
//      CloseHandle(priv->hThread);  
        // ɾ����Դ  
        delete priv;  
        thrd->m_Priv = NULL;  
    }  
}  
  
void OS_Thread::Msleep(int ms)  
{  
    ::Sleep(ms);  
}  
  
void OS_Thread::Sleep(int s)  
{  
    ::Sleep(s * 1000);  
}  
  
int OS_Thread::Routine()  
{  
    return 0;  
}  
  
#endif  //_WIN32  

��Linux�ϵ�ʵ��Thread_Linux.cpp ��ע��Thread_Linux.cpp��Thread_Win32.cpp��ͬʱ�����������Ŀ��ģ������Ѿ���#ifndef _WIN32�������ˣ���Ӱ����ı��룩

[cpp] view plaincopy��CODE�ϲ鿴����Ƭ�������ҵĴ���Ƭ

#ifndef _WIN32  
//#if 1  
#include <pthread.h>  
#include <unistd.h>  
#include <time.h>  
#include "Thread.h"  
  
  
struct OS_Thread_Priv  
{  
    pthread_t hThread;  
};  
  
OS_Thread::OS_Thread()   
: m_Priv(NULL)  
{  
}  
  
OS_Thread::~OS_Thread()  
{  
    if(m_Priv)  
    {  
        OS_Thread_Priv* priv = (OS_Thread_Priv*) m_Priv;  
        delete priv;  
    }  
}  
  
static void* OS_Thread_Proc_Linux(void* param)  
{  
    OS_Thread* thrd = (OS_Thread*) param;  
    thrd->Routine();  
    return NULL;  
}  
  
int OS_Thread::Run()  
{  
    // ����˽�нṹ  
    OS_Thread_Priv* priv = new OS_Thread_Priv;  
    if(!priv) return -1;  
  
    m_Priv = priv;  
  
    // �����߳�  
    if(pthread_create(&priv->hThread, NULL, OS_Thread_Proc_Linux, this) < 0)  
    {  
        delete priv;  
        m_Priv = NULL;  
        return -1;  
    }  
  
    return 0;  
}  
  
void OS_Thread::Join(OS_Thread* thrd)  
{  
    OS_Thread_Priv* priv = (OS_Thread_Priv*) thrd->m_Priv;  
    if(priv)  
    {  
        pthread_join(priv->hThread, NULL);  
  
        // ɾ����Դ  
        delete priv;  
        thrd->m_Priv = NULL;  
    }  
}  
  
void OS_Thread::Msleep(int ms)  
{  
    //::usleep(ms * 1000);  
    // ����ʹ��nanosleep����  
  
    timespec ts;  
    ts.tv_sec = ms / 1000;  
    ts.tv_nsec = (ms % 1000) * 1000000;  
    nanosleep(&ts, NULL);  
}  
  
void OS_Thread::Sleep(int s)  
{  
    ::sleep(s);  
}  
  
int OS_Thread::Routine()  
{  
    return 0;  
}  
  
  
  
  
#endif // ! _WIN32  
