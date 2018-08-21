//********************************************************************************
//�ļ���PMBaseAction.h
//���ߣ�feiren
//ʱ�䣺2011-8-20
//��;����װ�ؼ��ı�׼����
//�汾��1.0.0.0
//��ϵ��feirench@gmail.com
//********************************************************************************
#ifndef __PMBASEACTION_H__
#define __PMBASEACTION_H__

#include "PMSpaceAlgorithm.h"
#pragma once


class PMBaseAction
{
public:
	PMBaseAction(void);
	virtual ~PMBaseAction(void);
	PMVoid  Destroy							();					//�������
	PMVoid  UnDo							();					//��������
	PMVoid  ReDo							();					//��������
	TJSON * CreateCommand					(PMInt type);		//��������
	PMBool  ExcuteForward					(IntRect &OutObj);	//ִ��λ����ǰ
	PMBool  ExcuteBackward					(IntRect &OutObj);	//ִ��λ������	
	virtual PMVoid    ClearCommandList		(TJSONS &cmdList);  //ɾ��������ڴ����		
	virtual PMVoid    AddExcute				(TJSON *pAction);	//�������	
	virtual PMVoid    UnExcute				(TJSON &pAction)=0;	//����ִ������
	virtual PMVoid    Excute				(TJSON &pAction)=0;	//ִ������
	virtual PMVoid    AddPosition			(IntRect &InObj);//���λ����Ϣ
protected:	
	TJSONS					mUndoList;							//�����б�
	TJSONS					mRedoList;							//�����б�
	TJSONS					mPositionList;						//��ǰ��λ���б�
	PMInt					mIndex;								//�����λ���б�
};

#endif