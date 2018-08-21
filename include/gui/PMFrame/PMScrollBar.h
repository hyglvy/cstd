//********************************************************************************
//�ļ���PMScrollBar.h
//���ߣ�feiren
//ʱ��: 2011-8-20
//��;����װ���Ĺ�����ʵ�ַ���
//�汾��1.0.0.0
//��ϵ��feirench@gmail.com
//********************************************************************************
#ifndef __PMSCROLLBAR_H__
#define __PMSCROLLBAR_H__

#include "PMNavigator.h"
#include "PMThread.h"

#pragma once
class PMScrollBar;
enum PMSBType//����������
{
	SCROLL_NONE     = 0x0000,
	SCROLL_VERTICAL = 0x0001,
	SCROLL_HORIZON  = 0x0010,
	SCROLL_BOTH		= 0x0011,
	SCROLL_AUTO		= 0x0100,
};

class PMScrollBar:public IInput,public ScreenGDI
{
public:
	PMScrollBar();
	virtual ~PMScrollBar(void);
	PMVoid		Initialize(IInput *father,HWND hWnd,IntRect &ContentRect,TJSON *LaferScroll);
	PMVoid		LoadPicture();
	PMVoid		SetPageValue(PMInt PageValue)	{mPageValue = PageValue;}
	PMInt		GetPageValue()					{return mPageValue;}
	PMVoid		SetMax		(PMInt max);
	PMInt		GetMax		()					{return mMax;}
	PMVoid		SetActualMax(PMInt ActualMax)	{mActualMax = ActualMax;}
	PMInt		GetActualMax()					{return mActualMax;}
	PMVoid		SetStep		(PMInt step)		{mStep = step;}
	PMInt		GetStep		()					{return mStep;}
	PMVoid		SetLargeStep(PMInt LargeStep)	{mLargeStep = LargeStep;}
	PMInt		GetLargeStep()					{return mLargeStep;}
	PMVoid		SetValue	(PMInt value);
	PMInt		GetValue	()					{return mValue;}
	PMInt		GetScope	();
	PMBool		IsVertical  ();
	PMBool		IsHorition  ();
	PMVoid		ReCalcParameter(PMBool bSetMax);
	PMVoid		Show();
	PMVoid		Hide();	
	PMVoid		Draw();		
private:
	PMBool		OnLButtonDBLCLK	(TInputData *EventData) ;	//������˫��
	PMBool		OnPaint			(TInputData *EventData) ;	//���»���
	PMBool		OnEnter			(TInputData *EventData) ;   //������
	PMBool		OnLeave			(TInputData *EventData) ;	//������
	PMBool		OnMouseMove		(TInputData *EventData) ;	//�ؼ�����ƶ�
	PMBool		OnMouseWheel	(TInputData *EventData) ;	//�ؼ������ֹ���
	PMBool		OnLButtonDown	(TInputData *EventData) ;	//����������	
	PMBool		OnLButtonUp		(TInputData *EventData) ;	//�������ɿ�
	PMBool		OnSystemTimmer	(TInputData *EventData) ;   //��ʱ����Ϣ	
private:
	IntPoint	GetTiTaPosition	(TInputData *EventData);
	PMVoid		HandleVScroll	(PMInt iChange);
	PMVoid		HandleHScroll	(PMInt iChange);
	PMVoid      HandleScrollFinish();
	PMVoid		CalcAnchorRect();
	PMVoid		Clear();		
	PMVoid		ReCalcState		(PMInt x,PMInt y);
	PMVoid		ChangeState		(PMInt iState);
	PMVoid		MoveAnchor		(IntPoint &pt);	
	PMVoid		ChangeValue		(PMInt iChange,PMBool bRecalc=false);
	PMVoid		HandleChange	(PMInt iChange);
private:
	friend class PMScrollManager;
	enum TBTNState
	{
		BUTTON_DEFAULT,
		BUTTON_HOME_ENTER,
		BUTTON_HOME_PRESS,
		BUTTON_END_ENTER,
		BUTTON_END_PRESS,
		BUTTON_ANCHOR_ENTER,
		BUTTON_ANCHOR_PRESS,
		BUTTON_ANCHOR_HOME_SIDE_PRESS,
		BUTTON_ANCHOR_END_SIDE_PRESS,
	};
	PMSBType	mType;			//������������
	PMInt		mMax;			//����
	PMInt		mActualMax;		//����
	PMInt		mStep;			//��ť��
	PMInt		mLargeStep;		//����
	PMInt		mPageValue;		//ҳ��С
	PMInt		mValue;			//��ǰλ��
	PMInt		mHomeScope;
	PMInt		mEndScope;	
	PMInt		mOldX;
	PMInt		mOldY;	
	PMLong		mTicc;
	TBTNState   mState;
	const PMInt mSmallAchorLen;
	PMTasks		mTasks;
	
	IntRect		mTiTaRect;					//������GUI���Ʒ�Χ
	TJSON  *	mLaferScroll;				//�洢���ƵĹ���������
	IntRect		mRectButtonHome;			//�ϰ�ť��λ�÷�Χ
	IntRect		mRectButtonEnd;				//�°�ť��λ�÷�Χ
	IntRect		mRectAnchor;				//�����ӵ�λ�÷�Χ	

	PMPicture*	m_ScrollBackGround;			//����������ͼƬ����
	PMPicture*	m_ScrollButtonHome;			//�������ϰ�ťͼƬ����
	PMPicture*	m_ScrollButtonHomePress;	//�������ϰ�ťͼƬ����
	PMPicture*	m_ScrollButtonHomeEnter;	//�������ϰ�ťͼƬ������

	PMPicture*	m_ScrollButtonEnd;			//�������°�ťͼƬ����
	PMPicture*	m_ScrollButtonEndPress;		//�������°�ťͼƬ����
	PMPicture*	m_ScrollButtonEndEnter;		//�������°�ťͼƬ������

	PMPicture*	m_ScrollAnchor;				//������������ͼƬ����
	PMPicture*	m_ScrollAnchorPress;		//������������ͼƬ����
	PMPicture*	m_ScrollAnchorEnter;		//������������ͼƬ������
};

class PMScrollManager
{
public:	
	PMVoid		Initialize(IInput *father,HWND hWnd,IntRect &ContentRect,TJSON *jnScrollLafer,PMInt ScrollType);
	PMVoid		Show();
	PMScrollBar mHScrollBar;
	PMScrollBar mVScrollBar;
	PMNavigator mNavigator;
};
#endif