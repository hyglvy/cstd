//********************************************************************************
//�ļ���PMThread.h
//���ߣ�feiren
//ʱ�䣺2011-8-20
//��;����װ�̵߳���
//�汾��1.0.0.0
//��ϵ��feirench@gmail.com
//********************************************************************************
#include "PMType.h"
#pragma once

class PMThread//�߳�
{
	enum THREAD_STATE
	{
		THREAD_STATE_STOPED,
		THREAD_STATE_STOP,
		THREAD_STATE_RUN,
		THREAD_STATE_WAIT		
	};
public:
	PMThread(PMVoid);
	virtual ~PMThread(PMVoid);
	virtual PMVoid CallBack()=0;
	PMVoid AddTask(PMVoid *task);
	PMVoid Start();
	PMVoid Stop();
	PMUnlong GetThreadID();
private:
	static DWORD WINAPI WorkerProc( LPVOID lParam );
	PMVoid Lock();
	PMVoid UnLock();
	PMVoid Wait();
	PMVoid Wakeup();
	PMVoid Work();
protected:
	PMTasks				mTasks;
	PMInt				mThreadState;
	PMUnlong			mThreadId; 
	PMVoid*				mWorkEvent;
	PMVoid*				mStopEvent;
	CRITICAL_SECTION	mCsListAction;

};
