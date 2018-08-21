//********************************************************************************
//�ļ���PMEdit.h
//���ߣ�feiren
//ʱ�䣺2011-8-20
//��;����װ�����ı���ʵ��
//�汾��1.0.0.0
//��ϵ��feirench@gmail.com
//********************************************************************************
#ifndef __PMEDITOR_H__
#define __PMEDITOR_H__

#include "PMScrollBar.h"
#include "PMBaseAction.h"

#pragma once

#define RESERVEOFF 3
#define RESERVECONTENT 16
#define PICTURE_SECTION "\nPictures"
#define EDCONTENT_SECTION "Editor"

#define GETPOSITIONJSON(LD) &mJnData->At(LD.mJnIndex) 
//mBoundary�ǻ��ģ���Ϊ�������ű������һ����������߿�ȴ�ǲ����
class PMEdit:public IInput,ScreenGDI,PMBaseAction//�к��Ǵ�1��ʼ
{
	enum TextState      //-3�м�ѡ��,-2ǰ���ѡ��-1�����ѡ��0��ͨ��ȫ��1ѡ��2�༭
	{
		STATE_MIDDLE = -3,
		STATE_FRONT		 ,
		STATE_BACK		 ,
		STATE_NORMAL	 ,
		STATE_SELECTEDALL,
		STATE_PICTUREEDIT,
	};
	enum SelectionState //-1ȫѡ��0û��ѡ��1��Χ��2ͼƬ�༭
	{
		SELECTED_ALL = -1,
		SELECTED_NONE,
		SELECTED_SCOPE,
		SELECTED_PICTUREEDIT,
	};
	enum ECType//�༭����������
	{
		CONTENT_NONE			= 0x000000,
		CONTENT_TEXT			= 0x000001,
		CONTENT_PICTURE			= 0x000002,
		CONTENT_LINENUMBER		= 0x000004,
		CONTENT_LINEICON		= 0x000008,
		CONTENT_TEXTATTRIBUTE	= 0x000010,
		CONTENT_TEXTALIGN		= 0x000020,
	};
	enum ECPositionType//��ǰ�Ĳ��֣�-2�к�λ�á�-1ͼ��λ�á�0����λ��
	{
		POSITION_LINENUBER = -2,
		POSITION_LINEICON,
		POSITION_RETE,
	};
	struct ECElement//������ʾ�����ԭʼ
	{
		ECElement():mType(CONTENT_NONE),mJnIndex(0),mOffset(0),mCount(0),mStringID(NULL),mIntID(-1),mJnFont(NULL),mFontIndex(-1){}
		PMInt		mType;				//����,ֻ���������ͣ�text��picture
		IntRect		mBoundary;			//�߿�				
		PMInt		mJnIndex;			//���ݵ�����
		PMInt		mOffset;			//�ַ�ƫ��
		PMInt		mCount;				//�ַ�����
		PMChar*		mStringID;			//ͼƬ��MD5�����ơ��ַ�������
		PMInt		mIntID;				//GIFͼƬע���ID���ַ�������
		PMChar		mC;					//�����洢��ʱ����
		TJSON	   *mJnFont;			//����
		PMInt	    mFontIndex;			//���������,-1����ϵͳ����
		PMChar     *GetString()					  {mC=mStringID[mCount]; mStringID[mCount]=0; return mStringID;}
		PMVoid      Restore ()					  {mStringID[mCount]=mC;}
		PMVoid		Assign  (TTexToken *t)		  {mIntID=t->type; mStringID=t->p; mCount=t->len;}//��Assign����SetJson
		PMVoid		SetJson (TJSON *j)			  {mOffset=mStringID-j->ToCString();}
		PMVoid		Clear   ()					  {mType=CONTENT_NONE; mJnIndex=0; mOffset=0; mCount=0; mStringID=NULL; mIntID=-1; mJnFont=NULL; mFontIndex=-1;}
		PMVoid		SetBound(PMInt l,PMInt t,PMInt r,PMInt b) {mBoundary.left=l; mBoundary.top=t; mBoundary.right=r; mBoundary.bottom=b;}
		PMVoid		SetFont (PMInt index,TJSON *f){mFontIndex=index; mJnFont=f;}
		PMVoid		AddWidth(PMInt deta)		  {mBoundary.left=mBoundary.right;mBoundary.right+=deta;}
		PMVoid		SetPicture(PMInt w,PMInt h,TJSON *j);		
	};
	typedef vector<ECElement> ECElements;
	struct ECLine//������ʾ��������
	{
		ECLine():mRow(1),mY(0),mHeight(0),mWidth(0),mIconID(NULL),mGifID(-1){}
		PMInt		mRow;				//�к�
		PMInt		mY;					//y����
		PMInt		mHeight;			//�и�
		PMInt		mWidth;				//�п�
		PMChar*		mIconID;			//��ͼ��
		PMInt		mGifID;				//GIFͼƬע���ID
		ECElements  mElements;			//��ʾ����
	};
	typedef vector<ECLine*> ECLines;//������ʾ�ı༭������

	struct  Position//λ����Ϣ
	{
		Position():mSection(POSITION_RETE),mJnIndex(0),mOriOffset(0),mOffset(0),mJnFont(NULL),mFontIndex(-1),mFontHeight(0),mPositionX(0),mPositionY(0),mRow(0),mCol(0),mType(CONTENT_NONE){}
		PMInt		mSection;			//��ǰ�Ĳ��֣�-2�к�λ�á�-1ͼ��λ�á�0����λ��
		PMInt		mJnIndex;			//��Ӧ��json����λ��
		PMInt		mOriOffset;			//��Ӧ��json����ԭʼλ��ƫ��
		PMInt		mOffset;			//��Ӧ��json����λ��ƫ��
		TJSON	   *mJnFont;			//��Ӧ��json��������
		PMInt		mFontIndex;			//��Ӧ��json�������������
		PMInt		mFontHeight;		//��Ӧ��json��������߶�
		PMInt		mPositionX;			//��Ӧ��json����X����
		PMInt		mPositionY;			//��Ӧ��json����Y����
		PMInt		mRow;				//��Ӧ��json�������ڵ���
		PMInt		mCol;				//��Ӧ��json�������ڵ���
		PMInt		mType;				//���������
		
		PMVoid	ToJson(TJSON &jnFather)		  ;
		PMVoid	FromJson(TJSON &jnFather)	  ;
		PMVoid	Assign(ECElement *e)		  ;
		PMBool  operator !=(const TJSON &d)   ;
		PMBool  operator ==(const TJSON &d)	  ;
		PMBool	operator > (const Position &d){return mPositionY>d.mPositionY||(mPositionY==d.mPositionY&&mPositionX>d.mPositionX);}
		PMBool  operator !=(const Position &d){return !(mPositionX==d.mPositionX && mPositionY==d.mPositionY);}
		PMBool  operator ==(const Position &d){return mPositionX==d.mPositionX && mPositionY==d.mPositionY;}		
		PMVoid	Clear()						  {mSection=POSITION_RETE;mJnIndex=0;mOriOffset=0;mOffset=0;mJnFont=NULL;mFontIndex=-1;mFontHeight=0;mPositionX=0;mPositionY=0;mRow=0;mCol=0;}
		PMBool	Empty()						  {return mOffset==0 && mJnIndex==0 && mOriOffset==0;}
		IntPoint GetPosition()				  {return IntPoint(mPositionX,mPositionY);}
	};
	typedef vector<Position*> Positions;
	struct Selection //ѡ�񼯺� 
	{
		Selection():mType(SELECTED_NONE){}
		PMInt		mType;				//-1ȫѡ��0û��ѡ��1��Χ��2ͼƬ�༭
		Position	mFrom;				//ѡ�񼯵Ŀ�ʼλ��
		Position	mTo;				//ѡ�񼯵Ľ���λ��
		IntRect		mTopBound;			//ѡ�񼯵Ŀ�ʼ��Χ
		IntRect		mMidBound;			//ѡ�񼯵��м䷶Χ
		IntRect		mBotBound;			//ѡ�񼯵Ľ�����Χ
		PMVoid		Clear(){mType=SELECTED_NONE;mTopBound.clear();mMidBound.clear();mBotBound.clear();}
	};
	struct Caret    //���
	{
		Caret():mVisible(true),mbState(false),mLastX(0){}
		PMBool		mVisible;			//����Ƿ���ӣ����ǲ�����˸
		PMBool		mbState;			//��굱ǰ����ʾ״̬��1��ʾ����0����ʾ
		PMInt		mLastX;				//�����һ�ε�X�����������ơ����ơ���ҳ
		Position    mLocation;			//λ��
	};
	struct PictureResizeArea
	{
		PMInt	mIndex;					//�洢8�����λ�÷���,ѭ�������ң�����
		IntRect mNewBoundary;			//�µ�ͼƬ��Χ
		IntRect mBoundary;				//ԭʼ��ͼƬ��Χ
		IntRect mTop1;					//��1��Ĳ�׽��Χ
		IntRect mTop2;					//��2��Ĳ�׽��Χ
		IntRect mTop3;					//��3��Ĳ�׽��Χ
		IntRect mMiddle1;				//�м�1��Ĳ�׽��Χ
		IntRect mMiddle2;				//�м�2��Ĳ�׽��Χ
		IntRect mBottom1;				//��1��Ĳ�׽��Χ
		IntRect mBottom2;				//��2��Ĳ�׽��Χ
		IntRect mBottom3;				//��3��Ĳ�׽��Χ
	};
public://��������
	PMEdit(HWND hWnd,const RECT &ClientRect);
	virtual ~PMEdit(void);
	PMVoid  Initialize			(TJSON *jnFile);										//��ʼ���ı�������
	PMVoid  Show				(PMBool bDraw = false,PMBool bDrawScrollBar=true);		//��ʾ�ı���������
	//��������ʾ
	PMVoid  ShowEx				(TJSON *jnData ,TJSON *jnSystemFont);					//ֱ����ʾ�ı����ݣ����ڱ��������ʾ
	PMVoid	Clear				();														//�����ڴ�
	string  GetText				(TJSON *jnData);										//��ȡ�ı������������
	string  GetSelectedText		();														//��ȡѡ����������
	PMVoid  GetPack				(TBuffer &pack);										//��ȡ�������壬ͼƬ����������
	PMVoid  GetSelectedScopePack		(TBuffer &pack);								//��ȡ�������壬ͼƬ��ѡ������
protected://�ؼ����¼�����	
	PMBool  OnSetFocus			(TInputData *EventData) ;								//�ؼ��õ�����
	PMBool  OnLostFocus			(TInputData *EventData) ;								//�ؼ�ʧȥ����
	PMBool  OnMouseDrag			(TInputData *EventData) ;								//�ؼ���ʼ��ק
	PMBool  OnDragOver			(TInputData *EventData) ;								//�ؼ���ק����
	PMBool  OnMouseDrop			(TInputData *EventData) ;								//�ؼ���ק����
	PMBool  OnMouseMove			(TInputData *EventData) ;								//�ؼ�����ƶ�
	PMVoid  OnMouseMoveOutside	(TInputData *EventData) ;								//������ⲿ�ƶ�
	PMBool  OnMouseWheel		(TInputData *EventData) ;								//�ؼ������ֹ���	
	PMBool  OnLButtonDBLCLK		(TInputData *EventData) ;								//������˫��
	PMBool  OnRButtonDown		(TInputData *EventData) ;								//����Ҽ�����	
	PMBool  OnLButtonUp			(TInputData *EventData) ;								//�������ɿ�
	PMBool  OnRButtonUp			(TInputData *EventData) ;								//����Ҽ��ɿ�
	PMBool  OnKeyDown			(TInputData *EventData) ;								//���̷�ϵͳ������
	PMBool  OnKeyUp				(TInputData *EventData) ;								//���̷�ϵͳ���ɿ�
	PMBool  OnChar				(TInputData *EventData) ;								//������ͨ����
	PMBool  OnIMEChar			(TInputData *EventData) ;								//�������뷨����	
	PMBool  OnDropFiles			(TInputData *EventData) ;								//��explorer��ק�ļ����ؼ�	
	PMBool  OnCaretTimmer		(TInputData *EventData) ;								//������Ŷ�ʱ����Ϣ	
	PMBool  OnEnter				(TInputData *EventData) ;   							//������
	PMBool  OnLeave				(TInputData *EventData) ;   							//������
	PMBool  OnSize				(TInputData *EventData) ;								//��С�ı�
	PMBool  OnPaint				(TInputData *EventData) ;								//���»���
	PMBool  OnLButtonDown		(TInputData *EventData) ;								//����������
	PMBool  OnGifTimmer			(TInputData *EventData) ;								//��ʱ����Ϣ
private://�ؼ��Ĳ�������
	PMVoid  UnExcute			(TJSON &pAction);									    //����ִ������
	PMVoid  Excute				(TJSON &pAction);									    //ִ������	
	PMVoid  ClearCommandList	(TJSONS &cmdList);									    //ɾ��������ڴ����	

	//����Action����ʵ����
	PMVoid	CommandBack		();														    //ɾ��
	PMVoid	CommandDelete	();														    //ɾ��
	PMVoid	CommandCut		();														    //����
	PMVoid	CommandPaste	();														    //ճ��
	//���л��˵Ĳ���	
	PMVoid  DoEdit				(TJSON &pAction);										//��ǰɾ������
	PMVoid  DoPictureResize		(TJSON &pAction);										//ͼƬ�ı��С����
	//���˲���
			
	PMVoid  UnDoEdit			(TJSON &pAction);										//������ǰɾ������
	PMVoid  UnDoPictureResize	(TJSON &pAction);										//����ͼƬ�ı��С����

	//������û�л��˵Ĳ���
	PMVoid  DoCopy();																	//���Ʋ���
	PMVoid  DoSelectAll();																//ȫѡ����
	PMVoid  DoEscape();																	//ȡ������
	PMVoid  DoDown();																	//������Ʋ���
	PMVoid  DoUp();																		//������Ʋ���
	PMVoid  DoRight();																	//������Ʋ���
	PMVoid  DoLeft();																	//������Ʋ���
	PMVoid  DoPageDown();																//���·�ҳ����
	PMVoid  DoPageUp();																	//���Ϸ�ҳ����
	PMVoid  DoHome();																	//�ص����ײ���
	PMVoid  DoEnd();																	//�ص���β����
	PMVoid  DoInsert();																	//ɾ������
	PMVoid	DoSave();																	//��������

	PMVoid  ControlDelete();															//��βɾ��
	PMVoid	ControlBack();																//��ͷɾ��
	PMVoid  ControlHome();																//�ص����¿�ͷ
	PMVoid  ControlEnd();																//�ص�����ĩβ

	PMVoid  ShiftDown();																//ѡ����
	PMVoid  ShiftUp();																	//ѡ����
	PMVoid  ShiftRight();																//ѡ����
	PMVoid  ShiftLeft();																//ѡ����
	PMVoid  ShiftPageDown();															//ѡ����һҳ
	PMVoid  ShiftPageUp();																//ѡ����һҳ
	PMVoid  ShiftHome();																//ѡ������
	PMVoid  ShiftEnd();																	//ѡ��β

	PMVoid  AltRight();																	//�½���
	PMVoid  AltLeft();																	//�Ͻ���

	PMVoid	Refresh	(PMBool bShow);														//ˢ�£����¼����ı���ʾ���ݣ�����ʾ
private://�ؼ��¼�����������ʵ��	
	PMVoid		OnKeyChar					(TInputData *EventData);					//�����ַ������¼�
	PMVoid		OnMoveOutside				(TInputData *EventData);					//������ı����ⲿ�ƶ��¼�
	PMVoid		OnHScrollView				(PMInt iChange);							//ˮƽ����
	PMVoid		OnVScrollView				(PMInt iChange);							//��ֱ����
	PMVoid      OnIconAreaClick				(Position &LD);								//��ͼ�������
	PMVoid      OnReteAreaClick				(Position &LD,TInputData *EventData);		//���������
	PMVoid      OnLNAreaClick				(Position &LD,TInputData *EventData);		//�к������
	PMVoid		OnPictureClick				(Position &LD);								//ͼƬ�����
	PMVoid		OnSelectionAdd				(Position &LD);								//ѡ�����
	PMVoid		OnSelectionChange			(Position &LD);								//ѡ�񼯱��
	PMVoid		OnContentDrag				(Position &LD);								//���ݵ���ק
	PMVoid		OnPictureResize				(TInputData *EventData);					//ͼƬ��С�ı��¼�
	PMVoid		OnPictureResizeCompleted	(TInputData *EventData);					//ͼƬ��С�ı����
	PMVoid		OnMouseSelectOutside		(TInputData *EventData);					//������ı������ı�ѡ���¼�
	PMVoid		OnSelectedIdentical			();											//ѡ����ͬ���͵�����
	PMVoid		OnLineSelected				(TInputData *EventData);					//��ѡ��
	//����������������¼�
	PMVoid		OnHScroll					(PMInt iRange);								//ˮƽ����������
	PMVoid		OnVScroll					(PMInt iRange);								//��ֱ����������
	PMVoid		OnNavigate					(IntRect &Navigator);						//������ڸı�
private://����ʵ�ֹ��ܺ���
	IntRect		GetRetePosition				(IntRect  &Bound);							//����ʵ����ת����Rete����
	IntPoint	GetRetePosition				(IntPoint &Position);						//����ʵ����ת����Rete����	
	IntPoint	GetRetePosition				(PMInt x,PMInt y);							//����ʵ����ת����Rete����	
	IntRect		GetActualPosition			(IntRect  &Bound);							//��Rete����ת������ʵ����
	IntPoint	GetActualPosition			(IntPoint &Position);						//��Rete����ת������ʵ����	
	IntPoint	GetActualPosition			(PMInt x,PMInt y);							//��Rete����ת������ʵ����	

	PMVoid		CreateSWInfos				(TJSON *jnData,TJSON *jnSystemFont);		//������ʾ�ṹ	
	PMVoid		CreateAutoLineSWInfos		(TJSON *jnData,TJSON *jnSystemFont);		//������ʾ�ṹ	
	PMVoid		CreateSystemSWInfos			(TJSON *jnData,TJSON *jnSystemFont);		//������ʾ�ṹ		
	PMVoid		CreateCaretData				();											//���´������λ�ã�һ������ı���ͷ
	PMVoid		ScrollVertical				(PMInt Range);								//��ֱ����
	PMVoid		ScrollHorizon				(PMInt Range);								//ˮƽ����	
	PMBool		ScrollToPosition			(Position &LD);								//������ָ��λ����ʾ	
	PMBool		ScrollToLocation			(PMInt x,PMInt y);							//������ָ��λ����ʾ	
	PMVoid		ShowText					(ECLines::iterator &Line,ECElements::iterator &Element,PMInt State);//��ʾ����
	PMVoid		ShowPicture					(ECElement &SWInfo,PMInt State);			//��ʾͼƬ
	PMVoid		ShowLineIcon				(PMInt nLine);								//��ʾ�п�ͷ��ͼ��
	PMVoid		ShowLineNumber				(PMInt nLine);								//��ʾ�к�	
	PMVoid		DrawCaret					();											//���ƹ��
	PMVoid		DisplayCaret				();											//��ʾ�����ع��
	PMVoid		HideCaret					(HDC pdc);									//���ع��
	PMInt		GetType						(TJSON *jnItem);							//��ȡ��ǰjson�����ͣ��ı���ͼƬ��ͼ�ꡢ��������
	PMInt		GetCurrentRowHeight			();											//��ȡ��ǰ�и߶ȣ�Ӱ�����أ�����߶ȡ�ͼ��߶�
	ECLine*     CreateLine					();											//������
	PMVoid		DeleteLine					(PMInt  iLine);								//ɾ����
	PMInt		GetLineNumberWidth			(PMInt	iLine);								//��ȡ�кſ��
	PMInt		GetTotalWidth				(PMInt	iLine);								//��ȡ�ܹ������ݿ��
	PMInt		GetTotalHeight				();											//��ȡ�ܹ������ݸ߶�
	PMInt		GetHeaderWidth				(PMInt	iLine);								//��ȡ�ı���ͷ�Ŀ��
	PMVoid		CardingHeight				(ECLine *Line,PMInt h,PMInt w);				//���¼����еĸ߶�
	PMVoid		GetInitData					(TJSON *jnInitData);						//��ȡ�ı����ʼ������
	PMVoid		DrawImageFrame				(HDC pdc,IntRect &ImageBound);				//���Ʊ༭ͼƬ����߿�
	PMVoid		GetIterStartLine			();											//���㿪ʼ������	
	PMBool		IsInSelection				(Position &LD);								//λ���Ƿ���ѡ����
	PMVoid		SetSelection				(Position &LD);								//����ѡ��
	Position    GetPosition					(TInputData *EventData);					//����������ȡλ������
	Position    GetPosition					(PMInt x,PMInt y,PMInt MouseX);				//��Rete�����ȡλ������
	Position    GetOutsidePosition			(PMInt x);									//��ȡ��һ���ⲿ��ѡ�����
	PMVoid		AssignPosition				(Position &LD,ECLine *Line,ECElement *content,PMInt iCol,PMBool bLeft);//λ�ø�ֵ
	PMVoid		GetPositionData				(Position &LD,ECElement *content,PMInt x);  //��ȡ���ݵ�λ������
	PMBool		IsMouseInSelection			(TInputData *EventData);					//�ж�����Ƿ���ѡ��������
	PMInt		GetMouseArea				(TInputData *EventData);					//��ȡ�����������0��ͨ��������1�к�����2��ͼ������3ѡ������	
	PMInt		GetContentState				(ECElement *Emlement);						//-1��ѡ��0��ͨ��ȫ��1ѡ��2�༭	
	PMInt		GetPositionX				(Position &LD);								//��ȡλ�õ�X����
	PMVoid		RecalcPosition				(Position &LD);								//���¼���λ������
	PMVoid		SetBeginPosition			(Position &LD);								//����Ϊ��ʼ������
	PMVoid		SetEndPosition				(Position &LD);								//����Ϊ����������
	PMVoid		GetSystemFont				(Position &LD);								//���ı���û�����ݵ���ȴ����������ʱ��ʹ��
	PMVoid		SetLastFont					(Position &LD,PMInt Index);					//ר�Ŵ����ı�ĩβ����������
	PMVoid		CalcSelectionBound			();											//����ѡ���	
	PMVoid		SetCaret					(Position &LD,PMBool bSetLast = true,PMBool bSetX = true);//���ù��
	Position    GetPreLocation				(Position &LD);								//��ȡ��ǰλ�õ�ǰһ��λ��
	Position    GetNextLocation				(Position &LD);								//��ȡ��ǰλ�õĺ�һ��λ��
	Position    GetUpLocation				(Position &LD);								//��ȡ��ǰλ�õ���һ�е�λ��
	Position    GetDownLocation				(Position &LD);								//��ȡ��ǰλ�õ���һ�е�λ��
	Position    GetPageUpLocation			(Position &LD,PMInt DeltaY);				//��ȡ��һҳ��λ��
	Position    GetPageDownLocation			(Position &LD,PMInt DeltaY);				//��ȡ��һҳ��λ��
	Position    GetLineBeginLocation		(Position &LD);								//��ȡ�п�ͷ��λ��
	Position    GetLineEndLocation			(Position &LD);								//��ȡ�н�����λ��
	PMVoid		ScrollToPageDownLocaltion	(PMInt DeltaY,PMBool bShow);				//��������һҳ��λ��
	PMVoid		ScrollToPageUpLocaltion		(PMInt DeltaY,PMBool bShow);				//��������һҳ��λ��
	PMInt		GetPageDownDetaY			();
	PMInt		GetPageUpDetaY				();
	//�ı�ѡ��
	PMBool		VerticalSelected			(TInputData *EventData,PMInt iChange);		//��ֱѡ�񣬻����·������
	PMBool		HorizonSelected				(TInputData *EventData,PMInt iChange);		//ˮƽѡ�񣬻�ˮƽ�������
	PMBool		HScrollView					(PMInt iChange);							//ˮƽ����
	PMBool		VScrollView					(PMInt iChange);							//��ֱ����
	//ͼƬ�༭
	PMVoid		GetPictureResizeArea		(IntRect &ImageBound);						//��ȡͼƬ��С�ı�ķ�Χ����
	PMBool		InPictureResizeArea			(PMInt x,PMInt y);							//�жϲ�׽�Ƿ���ͼƬ��С�ı��λ��
	PMVoid		DrawPictureEditFrame		();											//����ͼƬ��С��Χ��
	PMVoid		ConvertPosition2Json		(TJSON &jnFather,Position &LD);				//��Positionת��ΪJson
	PMVoid		ConvertJson2Position		(TJSON &jnFather,Position &LD);				//��Jsonת��ΪPosition	
	IntRect		GetPositionRect				();											//��ȡ������ʾ����λ�õľ���	
	PMVoid		MoveView					(const IntRect &RT);						//�ƶ���ͼ
private://���ݲ���
	ECElement   *GetElement					(Position &LD);								//��ȡ��ǰλ�õĶ���
	ECElement   *GetPreElement				(Position &LD);								//��ȡ��ǰλ�õ�ǰһ������
	ECElement   *GetNextElement				(Position &LD);								//��ȡ��ǰλ�õĺ�һ������
	ECElement   *GetLastElement				();											//��ȡ���һ������
	PMVoid		CharacterMergeCommand		(PMChar *data);								//���ݺϲ�����
	PMVoid		SymbolAddCommand			(PMChar *data);								//�����������	
	TJSON*		GetLastCommand				();											//��ȡ���һ��ִ�еĿɺϲ�������	
	PMVoid		EditDelete					(PMInt iDirection);							//ɾ������
	PMVoid		EditContent					(Position &LD,TJSON *jnNew,TJSON *jnOld,PMInt iFrom);//�����滻
	PMVoid		EditContent					(TJSON &jnContent,PMInt iCommand);			//��ǰλ����ӣ�ѡ���滻��ѡ��ɾ��
	PMVoid		AddFont						(TJSON &jnNewData,ECElement *ElementTo,TJSON *jnOldData = NULL);//�������	
	PMVoid		ReCalcPosition				();											//���¼���λ��
	PMVoid		PushOldPosition				(TJSON &jnFather);							//����λ������
	PMVoid		PopOldPosition				(TJSON &jnFather);							//��ȡλ������
	PMInt		GetContentJsonIndex			(TJSON &jnContent);
	PMInt		GetContentJsonIndex			(PMInt iIndex);
	PMVoid		SetLDData					(Position &LD,TJSON &jnContent,ECElement *ElementFrom,PMInt iOffset,PMInt iIndex);
	PMBool		HasPosition					(Positions &LDS,PMInt jnIndex);
	PMVoid		ReCalcLinePosition			(Positions &LDS,ECLine *Line);
	PMBool		IsLastPosition				(Position &LD);
	PMBool		IsBeginPosition				(Position &LD);
	PMVoid		DealWithFontData			(Position LD,PMInt &iFrom);
	PMVoid		DealWithFontData			(Position LD,PMInt &iFrom,ECElement *ElementTo);
	Position    GetNextIdenticalPosition	(Position LD,PMInt &Type);
	Position    GetPreIdenticalPosition		(Position LD,PMInt &Type);
	PMBool		IsFont						(PMInt iIndex);
	PMBool		HasFont						(TJSON &jnContent);

	PMVoid		SetScrollParameter			();
	PMVoid		ShowScroll					();
private:
	PMInt		mStyle;						//�༭��ʽ������
	PMInt		mBKColor;					//�ı���ı�����ɫ
	PMInt		mTextFormat;				//�ı����뷽ʽ
	PMBool		mbEdit;						//�Ƿ�ɱ༭
	PMBool		mbAutoLine;					//�Ƿ��Զ�����
	PMBool		mbVScrollBar;				//�Ƿ���д�ֱ������
	PMBool		mbHScrollBar;				//�Ƿ����ˮƽ������
	PMInt		mLNBackColor;				//�кű�����ɫ
	PMInt		mTextSelectBKColor;			//����ѡ��ı�����ɫ
	PMInt		mHTextAxtra;				//����ˮƽ���
	PMInt		mVTextAxtra;				//���ִ�ֱ���
	PMInt		mVTextAxtraOffset;			//���ִ�ֱ���ƫ����
	TJSON*		mLNTextAttribute;			//�к�����
	PMInt		mVisibleX;					//��ǰ�ɼ�x�߼�����
	PMInt		mVisibleY;					//��ǰ�ɼ�y�߼�����		
	PMInt		mVisibleLine;				//��ǰ�ɼ��к�	
	PMInt		mHWheelSpeed;				//ˮƽ�����ٶ�
	PMInt		mVWheelSpeed;				//��ֱ�����ٶ�
	IntRect		mRectActual;				//��ʵ���ӵ���Դ��ڣ�mboundary����ʵ����
	IntRect		mReteRect;					//��ʵ���ӵ���Դ��ڣ�mboundary����ʵ����
	IntRect		mTextMargin;				//�����������ҵľ���
	IntSize		mSize;						//ʵ�ʵĳ���		
	ECLines	    mEDLines;					//���б������ֱ����ʾ������
	PMInt		mImageGroup;				//���༭����ͼƬ�������е�Gif�������
	PMInt		mIconDefaultWidth;			//����ϵ��
	PMInt		mIconWidth;					//ʵ��ͼ���С
private:
	PMScrollManager   mScrollManager;		//����������
	PMInt		mMouseState;				//���״̬
	PMCliboardManager*mCliboard;			//���������
	PMPictureManager *mPictureManager;		//ͼƬ������
	TColorFormula    *mSelectedFormula;		//ѡ�����ɫ�仯��ʽ	
	PMInt		mHeaderWidth;				//�༭��ͷ�Ŀ��,�����кš���ͼ�ꡢmMargin.left�Ŀ��
	TJSON	   *mjnFile;					//�ı��������ļ����߿������ͼƬ��mJnData
	TJSON	   *mProperty;					//�ı�������
	TJSON	   *mJnData;					//������ʼ���༭���������ݵ�json,����༭������
	TJSON	   *mJnInitData;				//�༭���������ݵ�json
	TJSON	   *mJnSystemFont;				//�༭��ϵͳ���ֵ�json
	PMInt		mXOffset;					//��ǰ��ʾ��Xƫ����
	PMInt		mYOffset;					//��ǰ��ʾ��Yƫ����
	PMInt		mContentWidth;				//��ǰ��ʾ�����ݿ��,�����кš���ͼ����ı�����
	PMInt		mContentHeight;				//��ǰ��ʾ�����ݸ߶�
	PMInt		mReteWidth;					//��ǰ��ʾ����Ч���ֿ��
	Caret		mCaret;						//���
	Position    mLastLD;					//���һ�εĶ�λλ��
	Selection   mSelection;					//ѡ��
	HRGN		mhRgnReteRect;				//�������ü���
	PictureResizeArea mPictureResizeArea;	//�༭ͼƬ�������״�ı��
};

#endif