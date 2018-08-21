//********************************************************************************
//�ļ���PMNavigator.h
//���ߣ�feiren
//ʱ�䣺2011-8-20
//��;����װת����,����ȿ���ת����Ӧ�ĵط�
//�汾��1.0.0.0
//��ϵ��feirench@gmail.com
//********************************************************************************
#ifndef __PMNAVIGATOR_H__
#define __PMNAVIGATOR_H__

#include "PMScreenGDI.h"
#include "PMInputManager.h"
#include "PMScrollConst.h"
#include "PMPictureManager.h"
#pragma once

class PMNavigator:public IInput,public ScreenGDI
{
public:
	PMNavigator();
	virtual ~PMNavigator();
	PMVoid		Initialize		(IInput *father,HWND hWnd,IntRect &ClientRect,TJSON *LaferNavigator);
	PMVoid		SetScope		(IntSize &Scope);			//���÷�Χ
	PMVoid		SetScopeWidth	(PMInt ScopeWidth);			//���÷�Χ
	PMVoid		SetScopeHeiht	(PMInt ScopeHeight);		//���÷�Χ
	PMVoid		SetNavigator	(IntRect &Navigator);		//������ʾ����
	PMVoid		SetNavigatorX	(IntSize &NavigatorX);		//������ʾ����
	PMVoid		SetNavigatorY	(IntSize &NavigatorY);		//������ʾ����
	PMVoid		Draw();	
	PMVoid		Show();
private:
	PMBool		OnLButtonDown	(TInputData *EventData);	//����������		
	PMBool		OnMouseMove		(TInputData *EventData);	//����ƶ�
	IntPoint	GetTiTaPosition	(TInputData *EventData);
private:
	IntRect		GetDrawRect();
	PMVoid		CalcNavigator(PMInt x,PMInt y);
	PMVoid		Clear();
	IntRect		mTitaRect;
	IntSize		mScope;
	IntRect		mNavigator;
	PMPicture*	m_NavigatorBackGround;			//ת��������
	PMInt		mLineColor;
	TJSON	*	mLaferNavigator;
};

#endif
