//********************************************************************************
//�ļ���PMScreenGDI.h
//���ߣ�feiren
//ʱ�䣺2011-8-20
//��;����װ������GDI��ʾ������lafer�㷨��ȫ��ö�ٶ���
//�汾��1.0.0.0
//��ϵ��feirench@gmail.com
//********************************************************************************
#ifndef __PMSCEENGDI_H__
#define __PMSCEENGDI_H__

#include "PMSpaceAlgorithm.h"
#include "PMGridConst.h"
#include "ximage.h"


#pragma comment (lib,"CXImage.lib")

#pragma once

#define TABLENGHT 4

enum PMType
{
//1��*******************************ʵ�������***********************************
	PMENTITY_POINT=1,	//��
	PMENTITY_LINE,		//��
	PMENTITY_POLYLINE,	//����
	PMENTITY_TRIANGLE,	//������
	PMENTITY_RECTANGLE,	//����	
	PMENTITY_POLYGON,	//�����
	PMENTITY_ELLIPSE,	//��Բ
	PMENTITY_PIE,		//����
	PMENTITY_SMOOTINGRECT,	//�������
//2��*******************************��Ԫ������***********************************
	PMCELL_SYSTEM=0,	//ϵͳ���,���Ǿ������ֵ�ͷ���к�
	PMCELL_USER,		//�û�����
	PMCELL_FIX,			//�����Ա༭�ı��
	PMCELL_TEXT,		//�ı����
	PMCELL_PICTURE,		//ͼƬ���ͼƬ��Ϊ������ǰ����ʾ���֣�
	PMCELL_FUNCTION,	//���ܱ�������غ�����
	//==========================�ȵ���ʵ��=============================
	PMCELL_TREE=0,		//�����
	PMCELL_LIST,		//�б�
	PMCELL_COMBOLBOX,	//��Ͽ�
	PMCELL_BUTTON,		//��ť
	PMCELL_TIME,		//ʱ��ؼ�
	PMCELL_DATE,		//���ڿؼ�
	PMCELL_CKECKBOX,	//ѡ���
	PMCELL_TABBOX,		//ҳ���
	PMCELL_PROGRESS,	//������
	PMCELL_CHART,		//ͼ��
	PMCELL_MIND,		//˼ά��ͼ
	PMCELL_GRAPHIC,		//��ͼ��
	PMCELL_MEDIA,		//����
	PMCELL_VIDIO,		//��Ƶ
	PMCELL_ORGE3D,		//OGRE 3D����
//3��******************************��Ԫ��״̬************************************
	PMCELL_DEFAULT			=0x000000,
	PMCELL_FOCUSED			=0x000001,
	PMCELL_SELECTED			=0x000010,
	PMCELL_ENTER			=0x000100,
	PMCELL_MODIFIED			=0x001000,
//4��*******************************��Ԫ���߿�����***********************************
	PMCELL_LINE_All			=0x000000,
	PMCELL_LINE_UP			=0x000001,
	PMCELL_LINE_DOWN		=0x000010,
	PMCELL_LINE_LEFT		=0x000100,
	PMCELL_LINE_RIGHT		=0x001000,
	PMCELL_LINE_UP_CROSS	=0x010000,
	PMCELL_LINE_DOWN_CROSS	=0x100000,
	PMCELL_GROUND			=0x000002,
//5��*******************************������ĵ�ǰ״̬***********************************
	PMCELL_MOUSE_DEFAULT	=0x000000,
	PMCELL_MOUSE_SELECTED	,
	PMCELL_MOUSE_RESIZENS	,
	PMCELL_MOUSE_RESIZEWE	,
	PMCELL_MOUSE_DRAG		,
	PMCELL_MOUSE_COPY		,

//6��********************************�༭������״̬**********************************
	PMEDITOR_MOUSE_DEFAULT	=0x000000,
	PMEDITOR_MOUSE_SELECTED ,
	PMEDITOR_MOUSE_LINESELECT,
	PMEDITOR_MOUSE_BEGINDRAG,
	PMEDITOR_MOUSE_DRAG		,	
	PMEDITOR_MOUSE_PICTUREEDIT,
	PMEDITOR_MOUSE_PICTURERESIZE,

//7��********************************�༭�������״̬**********************************
	PMEDITOR_COMMAND_DEFAULT	= 0x000000,
	PMEDITOR_COMMAND_CHARACTERMERGE	,
	PMEDITOR_COMMAND_DELETE			,
	PMEDITOR_COMMAND_EDIT			,

	PMEDITOR_COMMAND_PICTURERESIZE	,
	PMEDITOR_COMMAND_DRAG			,
};
class ScreenGDI;
class PMPicture
{
public:
	PMPicture();
	~PMPicture();
	void ReleaseBuffer();
public:
	HBITMAP mhBitmap;
	IntSize mSize;	
	IntRect mRect;
	//TJSON   *mJson;
};

class PMPictures
{
public:
	PMPictures();
	~PMPictures();
	PMPicture* operator[](PMInt index);
	void ClearPictures();
public:
	vector<PMPicture*> mPictures;	
};

class PMLaferLexer
{
public:
	TJSON *CreateLaferJson		(const string &laferdata);							//��lafer����ת����JSON�ڴ�����
	string CreateLaferCode		(TJSON &laferJson);									//���ڴ�����ת�����ַ���
private:
	PMVoid ParseEntity		(TJSON &laferJson,TWordLex &lex);
	PMVoid ParseRect		(TJSON &laferJson,TWordLex &lex);
	PMVoid ParseFrame		(TJSON &laferJson,TWordLex &lex);
	PMVoid ParseBody		(TJSON &laferJson,TWordLex &lex);
	PMVoid ParsePoint		(TJSON &laferJson,TWordLex &lex);
	PMVoid ParseLine		(TJSON &laferJson,TWordLex &lex);
	PMVoid ParsePolyLine	(TJSON &laferJson,TWordLex &lex);
	PMVoid ParseTriangle	(TJSON &laferJson,TWordLex &lex);
	PMVoid ParseRectangle	(TJSON &laferJson,TWordLex &lex);
	PMVoid ParseSmootingRect(TJSON &laferJson,TWordLex &lex);
	PMVoid ParseEclipse		(TJSON &laferJson,TWordLex &lex);
	PMVoid ParsePolygon		(TJSON &laferJson,TWordLex &lex);
	PMVoid ParsePie			(TJSON &laferJson,TWordLex &lex);

	string EntityToCode		(TJSON &laferJson);
	string FrameToCode		(TJSON &laferJson);
	string BodyToCode		(TJSON &laferJson);
	string PointToCode		(TJSON &laferJson);
	string LineToCode		(TJSON &laferJson);
	string PolyLineToCode	(TJSON &laferJson);
	string TriangleToCode	(TJSON &laferJson);
	string RectangleToCode	(TJSON &laferJson);
	string SmootingRectToCode(TJSON &laferJson);
	string EllipseToCode	(TJSON &laferJson);
	string PolygonToCode	(TJSON &laferJson);
	string PieToCode		(TJSON &laferJson);
	string ColorToCode		(TJSON &laferJson);


	PMVoid SetJsonValue(TJSON &laferJson,const string &strValue);
	PMVoid SetJsonValue(TJSON &laferJson,TWordLex &lex);
	PMVoid SetJsonColor(TJSON &laferJson,TWordLex &lex);
	string GetJsonValue(TJSON &laferJson);
	string GetJsonColor(TJSON &laferJson);
};

struct PenData
{
	PenData():mHDC(NULL),mColor(0),mWidth(1),mStyle(0){}
	PMBool Equal(HDC hdc,PMColor iColor, PMInt iWidth, PMInt iStyle);
	PMVoid Set(HDC hdc,PMColor iColor, PMInt iWidth, PMInt iStyle);
	HDC		mHDC;
	PMColor mColor;
	PMInt	mWidth;
	PMInt	mStyle;
};

struct  TColorFormula
{
	static TColorFormula *GetSingleton();
	TColorFormula():RBase(68),RNumerator(89),GBase(71),GNumerator(90),BBase(80),BNumerator(90){};
	PMVoid  Initialize(TJSON *json);
	PMColor GetColor(PMColor sourceColor);	
	BYTE RBase;
	BYTE RNumerator;
	BYTE GBase;
	BYTE GNumerator;
	BYTE BBase;
	BYTE BNumerator;
};
//�����װGDI��ͼ��ÿ�ζ�ֱ�ӻ����ڴ��bitmap�ϣ�
//ֱ������DrawFrame�Ž����ݻ��Ƶ���Ļ��
enum TTEXTFORMAT
{
	TF_VTOP		=0x0001,
	TF_VCENTER	=0x0002,
	TF_VBOTTOM	=0x0004,
	TF_HLEFT	=0x0008,
	TF_HCENTER	=0x0010,
	TF_HRIGHT	=0x0020
};
class ScreenGDI
{
public:
	ScreenGDI();
	virtual ~ScreenGDI(void);
	static PMVoid SetInstance	(HINSTANCE hInstance);
	PMVoid Initialize			(HWND hWnd,const RECT &ClientRect);
	PMVoid SetCurrentDC			(HDC currentDC);									//���õ�ǰDC
	PMVoid DrawFrame			(const IntRect *rect=NULL);							//��ScreenGDI���������ݻ��Ƶ���Ļ��
	PMVoid WindowResize			(const RECT &clientRect);							//�������ô���Ĵ�С
	PMVoid WindowResize			(const IntRect &clientRect );						//�������ô���Ĵ�С
	HDC    GetDC				();													//��ȡ��ǰ��DC
	PMVoid ClearBackGround		(PMInt iColor=-1);									//��������
	PMVoid SetNullPen			();													//���ÿջ���
	PMVoid SetPen				(PMColor color = RGB(0,0,0),PMInt width = 1,PMInt style = 0);//���û���
	PMVoid SetHatchPen			(PMColor color,PMInt width ,PMInt style );			//������ʽ����
	PMVoid SetBrush				(PMColor color = RGB(255,255,255));					//������ɫ��ˢ
	PMVoid SetHatchBrush		(PMInt fnStyle,PMColor color = RGB(255,255,255));	//������ʽ��ˢ
	PMVoid SetPictureBrush		(PMBitmap hbmp);									//����ͼƬ��ˢ
	PMVoid SetSystemFont		();													//����ϵͳ���е�����
	PMVoid SetFont				(TJSON *jnFont);									//����ϵͳ���е�����
	PMVoid CalcFontWidth		();													//���㵱ǰ����Ŀ��
 	PMVoid SetTextAxtra			(PMInt VAxtra,PMInt HAxtra);						//����������ʾ�ľ���
	PMInt  GetStringWidth		(const PMChar *data,PMInt len);

	PMVoid LineExactly			(PMInt x1,PMInt y1,PMInt x2,PMInt y2,PMInt width);	//��ȷ����
 	PMVoid DrawPicture			(const PMPicture *picture,const IntRect &rect);		//����ͼƬ
 	PMVoid DrawLine				(const IntPoint &start,const IntPoint &end);		//��������
	PMVoid DrawLine				(const IntPoints &points);							//������������
 	PMVoid DrawDoubleLine		(const IntPoint &start,const IntPoint &end);		//����˫��
 	PMVoid FillRetangle			(const IntRect &rect);								//�������ݣ������Ա߿����ɫ
	PMVoid DrawRetangle			(const IntRect &rect);								//���Ʊ߿򣬺����������
	PMVoid Retangle				(const IntRect &rect);								//���Ʊ߿򡢺�����
	PMVoid ShowNumber			(PMInt number,PMInt group=0);
	PMVoid TextOutFormat		(const PMChar *data,PMInt len,const IntRect &rect,PMInt format=TF_VCENTER|TF_HCENTER);

	PMVoid PushPicture			(const IntSize &pictureSize);						//����λͼ������Ϊ��ǰDC
	PMPicture* PopPicture();														//��ȡλͼ������ǰDC�ָ�ԭ��

	//���Ʊ߿򣬸�ʽ���ݺ�������ʽ��
	PMPicture* CreatePictur		 ( const IntSize &pictureSize,PMColor color );		//����һ����ɫ��ͼƬ
	PMPicture* CreateLaferPicture(const IntSize &pictureSize,
								 const string &laferdata,const string &name);		//������������lafer���ݵ�ͼƬ,ֻ�ܴ�Entity��json
	PMPicture* CreateLaferPicture(const IntSize &pictureSize,
								  TJSON &laferJson,const string &name);				//������������lafer���ݵ�ͼƬ,ֻ�ܴ�Entity��json	
	PMPicture* CreateLaferFillPicture(const IntSize &pictureSize,
								 TJSON &laferJson,const string &name);				//�����������Ʊ߿��ͼƬ,ֻ�ܴ�Entity��json
	PMVoid	   DrawLaferFrame	(TJSON &laferJson,const IntRect &laferRect,
								 const string &name);								//���������ƽ��л�����Ļ,ֻ�ܴ�Entity��json
	PMVoid	   DrawLaferLineFrame(TJSON &laferJson,const IntRect &laferRect,
								 const string &name);								//���������ƽ��߶λ��Ƶ���Ļ,ֻ�ܴ�Entity��json
	TJSON*	   CreateLaferJson	(const string &laferdata);							//��lafer����ת����JSON�ڴ�����
	string     CreateLaferCode	(TJSON &laferJson);									//���ڴ�����ת�����ַ���
	IntRect    GetRect			(TJSON &laferJson,const string &name ="RECT");									//��ȡ�߿��С	
	

protected:
	PMVoid InitBuffer();
	PMVoid ReleaseBuffer();
	PMInt  GetValueX			(TJSON &laferJson);									//��lafer json��ȡx����
	PMInt  GetValueY			(TJSON &laferJson);									//��lafer json��ȡy����
	PMInt  GetWidthValue		(TJSON &laferJson);									//��lafer json��ȡ���
	PMVoid DrawLaferBody		(TJSON &laferJson);									//����lafer֡����
	PMVoid DrawLaferObjects		(TJSON &laferJson);									//����lafer����ʵ��
	PMVoid DrawLaferPoint		(TJSON &laferJson);									//����lafer��
	PMVoid DrawLaferLine		(TJSON &laferJson);									//����lafer��
	PMVoid DrawLaferPolyLine	(TJSON &laferJson);									//����lafer����
	PMVoid DrawLaferRectangle	(TJSON &laferJson);									//����lafer����ɫ����
	PMVoid DrawLaferSmootingRect(TJSON &laferJson);									//����lafer����
	PMVoid DrawLaferTriangle	(TJSON &laferJson);									//����lafer������
	PMVoid DrawLaferPolygon		(TJSON &laferJson);									//����lafer�����
	PMVoid DrawLaferEllipse		(TJSON &laferJson);									//����lafer��Բ
	PMVoid DrawLaferPie			(TJSON &laferJson);									//����lafer����
	PMInt  SetParameter			(TJSON &wJson,TJSON &cJson,TJSON *bgcJson=NULL);	//�����߿�	
	HDC		mHdcBitmap;
	HDC		mHdcPicture;
	HDC		mCurrentHDC;
	HBITMAP mBitmap;	
	HBRUSH  mhWhiteBrush;
	HPEN	mhWhitePen;
	HBITMAP mOldBitmap;
	HBRUSH  mhOldBrush;
	HPEN	mhOldPen;
	HBRUSH  mhOldPictureBrush;
	HPEN	mhOldPicturePen;
	HFONT   mhOldFont;
	HFONT   mhSystemFont;

	HFONT   mhFont;
	HPEN	mhPen;
	HPEN    mNullPen;
	HBRUSH  mNullBrush;
	HBRUSH	mhBrush;	
	PenData mOldPendata; 

	PMInt	mVAxtra;
	PMInt	mHAxtra;
	//PMColor mOldPenColor;
	PMColor mOldBrushColor;
	PMVoid* mObjects[20];
	TJSON * mJnFont;
	PMInt   mFontTags[260];
	PMInt   mWideFontWidth;
	static const PMColor	mSystemFixLineColor  = 6974058;
	static const PMColor	mSystemGridLineColor = 10921638;
public:
	static  HINSTANCE mHinstance;
	HWND	mHWnd;
	HDC		mHdc;
	IntRect mRect;
	IntRect mLaferRect;
	IntRect mWindowRect;
	PMInt	mFontWidth;
	PMInt	mFontHeight;
};

#endif