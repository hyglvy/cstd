// NcEditDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CNcEditDemoApp:
// �йش����ʵ�֣������ NcEditDemo.cpp
//

class CNcEditDemoApp : public CWinApp
{
public:
	CNcEditDemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNcEditDemoApp theApp;
