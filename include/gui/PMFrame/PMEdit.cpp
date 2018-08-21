#include "StdAfx.h"
#include "PMEdit.h"

#define DEFAULTLINENUMBERWIDTH 18

PMVoid PMEdit::ECElement::SetPicture( PMInt w,PMInt h,TJSON *j )
{
	mType			 = CONTENT_PICTURE;			
	mBoundary.bottom = h;
	mStringID		 = j->At(0).ToCString();
	mCount			 = 1;		
	mOffset			 = 0;
	AddWidth(w);
}

PMVoid PMEdit::Position::ToJson( TJSON &jnFather )
{
	jnFather.Push(mSection);
	jnFather.Push(mJnIndex);
	jnFather.Push(mOriOffset);
	jnFather.Push(mOffset);
	jnFather.Push(mFontIndex);
	jnFather.Push(mFontHeight);
	jnFather.Push(mPositionX);
	jnFather.Push(mPositionY);
	jnFather.Push(mRow);
	jnFather.Push(mCol);
	jnFather.Push(mType);
}

PMVoid PMEdit::Position::FromJson( TJSON &jnFather )
{
	mSection	= jnFather.At(0).ToInt();
	mJnIndex	= jnFather.At(1).ToInt();
	mOriOffset	= jnFather.At(2).ToInt();
	mOffset		= jnFather.At(3).ToInt();
	mFontIndex	= jnFather.At(4).ToInt();
	mFontHeight = jnFather.At(5).ToInt();
	mPositionX	= jnFather.At(6).ToInt();
	mPositionY	= jnFather.At(7).ToInt();
	mRow		= jnFather.At(8).ToInt();
	mCol		= jnFather.At(9).ToInt();
	mType		= jnFather.At(10).ToInt();
}

PMBool PMEdit::Position::operator!=( const TJSON &d )
{
	return !(mJnIndex==d.At(1).ToInt()&&(mOriOffset+mOffset)==(d.At(2).ToInt()+d.At(3).ToInt()));
}

PMBool PMEdit::Position::operator==( const TJSON &d )
{
	return mJnIndex==d.At(1).ToInt()&&(mOriOffset+mOffset)==(d.At(2).ToInt()+d.At(3).ToInt());
}

PMVoid PMEdit::Position::Assign( ECElement *e )
{
	mType=e->mType;
	mJnIndex=e->mJnIndex;
	mJnFont=e->mJnFont;
	mOriOffset=e->mOffset;
	mFontIndex=e->mFontIndex;
	mPositionY=e->mBoundary.bottom;
}

PMEdit::PMEdit(HWND hWnd,const RECT &ClientRect):mStyle(0),mIconDefaultWidth(16),mHeaderWidth(0),mHTextAxtra(0),mVTextAxtra(3),mjnFile(NULL),
mJnData(NULL),mJnInitData(NULL),mJnSystemFont(NULL),mMouseState(PMEDITOR_MOUSE_DEFAULT),mhRgnReteRect(NULL)
{	
	mPictureManager   = PMPictureManager::GetSingleton();
	mSelectedFormula  = TColorFormula::GetSingleton();
	mCliboard		  = PMCliboardManager::GetSingleton();
	mImageGroup		  = mPictureManager->CreateImageRoup();
	mVTextAxtraOffset = PMSpaceAlgorithm::Up((PMDouble)mVTextAxtra/2);
	ScreenGDI::Initialize(hWnd,ClientRect);
	SetTextAxtra(mVTextAxtra,mHTextAxtra);
	mProperty = TJSON::ReadFile("property.json");
}

PMEdit::~PMEdit(void)
{
	PMBaseAction::Destroy();
	Clear();
	if(mhRgnReteRect)DeleteObject(mhRgnReteRect);
	TJSON::DeleteJSON(mProperty);
}

PMVoid PMEdit::Initialize(TJSON *jnFile )
{		
	Clear();
	mjnFile			= jnFile;
	mJnData			= &mjnFile->At(EDITDATA);
	mJnInitData		= &mProperty->At(EDIT_ATTRIBUTE);
	mJnSystemFont	= &mProperty->At(SYTEM_FONT);	
	GetInitData(mJnInitData);	
}

PMVoid PMEdit::Refresh(PMBool bShow)
{
	bool bCreate = mEDLines.empty();
	Clear();
	mPictureManager->SetGifGroup(mImageGroup);
	SetFont		  (mJnSystemFont);	
	mIconWidth   = 0;
	if(mStyle&CONTENT_LINEICON) mIconWidth = mIconDefaultWidth>mFontHeight?mIconDefaultWidth:mFontHeight;
	CreateSWInfos (mJnData,mJnSystemFont);			
	mContentWidth = mBoundary.width()  - mMargin.left - mMargin.right;
	mContentHeight= mBoundary.height() - mMargin.top - mMargin.right;
	if(mSize.height<=mContentHeight) 
	{
		mVisibleY = 0;
	}else if((mSize.height>mContentHeight)&&(mVisibleY>mSize.height - mContentHeight ))
	{
		mVisibleY = mSize.height - mContentHeight ;
	}
	if(mSize.width<=mContentWidth) 
	{
		mVisibleX = 0;
	}else if((mSize.width>mContentWidth)&&(mVisibleX>mSize.width - mContentWidth ))
	{
		mVisibleX = mSize.width - mContentWidth ;
	}
	GetIterStartLine();		
	if(bCreate)CreateCaretData();
	else ReCalcPosition();
	if(bShow)
	{
		Show(true);
	}
}

PMVoid PMEdit::Show(PMBool bDraw,PMBool bDrawScrollBar)
{
	if(!mJnData)return ;	
	//1�����㼸������
	mHeaderWidth    = GetHeaderWidth(mVisibleLine);		
	mReteWidth      = mContentWidth  - mHeaderWidth - mMargin.right;
	mXOffset	    = mBoundary.left + mHeaderWidth - mVisibleX;
	mYOffset	    = mBoundary.top  + mMargin.top  - mVisibleY;
	mReteRect.left  = mBoundary.left + mHeaderWidth;
	mReteRect.top   = mBoundary.top  + mMargin.top;
	mReteRect.right = mReteRect.left + mReteWidth;
	mReteRect.bottom= mBoundary.bottom  - mMargin.bottom;	
	//2����������
	ClearBackGround(mBKColor);
	//3������ͼƬ
	mPictureManager->SetGifGroup(mImageGroup);
	//4����ʾ�к���ͼ��
	ShowLineNumber(mVisibleLine);
	SetFont		  (mJnSystemFont);
	ShowLineIcon  (mVisibleLine);
	//6����ʾ����
	if(mhRgnReteRect)DeleteObject(mhRgnReteRect);
	mhRgnReteRect = CreateRectRgn(mReteRect.left,mReteRect.top,mReteRect.right,mReteRect.bottom);
	
	ECLines::iterator  it		= mEDLines.begin() + mVisibleLine;
	ECLines::iterator  itEnd	= mEDLines.end();
	ECElements::iterator iter	;
	ECElements::iterator iterEnd ;	
	PMInt	  height = mVisibleY + mContentHeight;
	PMInt	  rwidth = mVisibleX + mReteWidth ;	

	ECLine *line;
	ECElement  *content;
	SelectClipRgn(mCurrentHDC,mhRgnReteRect);
	
	while (it!=itEnd)
	{
		line	= (*it);
		iter    = line->mElements.begin();
		iterEnd = line->mElements.end();
		for (; iter != iterEnd; iter++)
		{
			content = &(*iter);
			switch(content->mType)
			{
			case CONTENT_TEXT:
				if(content->mBoundary.right >= mVisibleX&&content->mBoundary.left<rwidth) ShowText(it,iter,GetContentState(content));
				break;
			case CONTENT_PICTURE:
				if(content->mBoundary.right >= mVisibleX&&content->mBoundary.left<rwidth) ShowPicture(*content,GetContentState(content));
				break;
			}			
		}	
		if(height < line->mY + line->mHeight)break;
		it++;
	}
	//7������ѡ��ͼƬ
	mPictureManager->DrawEditImage(mCurrentHDC);
	//8������ͼƬ�༭����С
	if(mSelection.mType == SELECTED_PICTUREEDIT)
	{
		GetPictureResizeArea(GetActualPosition(mSelection.mTopBound));
	}
	SelectClipRgn(mCurrentHDC,NULL);
	//9�����ƹ�����
	if(bDrawScrollBar)
	{
		ShowScroll();
	}
	if(bDraw)
	{		
		DrawFrame(&mBoundary);
		DisplayCaret();	
	}
}

PMVoid PMEdit::CreateSWInfos( TJSON *jnData ,TJSON *jnSystemFont)
{	
	if(mbAutoLine)
	{
		CreateAutoLineSWInfos(jnData,jnSystemFont);
	}else
	{
		CreateSystemSWInfos(jnData,jnSystemFont);
	}	
}


#define ADDLINE CardingHeight(Line,MaxHeight,SWInfo.mBoundary.right);\
PMInt y		  = Line->mY + Line->mHeight;\
Line		  = CreateLine();\
Line->mY      = y;\
SWInfo.mBoundary.left  = 0;\
SWInfo.mBoundary.right = 0;\
MaxHeight	  = GetCurrentRowHeight();\
TotalWidth    = GetTotalWidth(mEDLines.size());

PMVoid PMEdit::CreateAutoLineSWInfos( TJSON *jnData,TJSON *jnSystemFont )
{
	//�����������	
	TTextWordLex WordLex;
	TTexToken   *token;
	ECElement    SWInfo  ;		
	TJSON	    *jnItem  ;
	//SIZE	     size;
	ECLine      *Line	    = CreateLine();
	PMInt	     CurHeight  = 0;
	PMInt	     CurWidth   = 0;
	PMInt	     jnSize	    = jnData->GetChildCount();
	PMInt	     MaxHeight  = GetCurrentRowHeight();	
	PMInt	     TotalWidth = GetTotalWidth(0);
	PMBool		 bAdd		= false;
	PMChar		*strData;
	mSize.clear();
	SWInfo.SetFont(-1,mJnSystemFont);
	CalcFontWidth();
	//�������е�����
	while(SWInfo.mJnIndex<jnSize)
	{		
		jnItem = &(jnData->At(SWInfo.mJnIndex));
		//2����ͼ��
		switch(GetType(jnItem))
		{
		case CONTENT_TEXT:
			{																
				bAdd = false;	
				strData = jnItem->ToCString();
				WordLex.SetData(strData);
				SWInfo.mBoundary.bottom  = GetCurrentRowHeight();
				SWInfo.mType		     = CONTENT_TEXT;
				token = WordLex.GetToken();
				while (token->type)
				{
					switch(token->type)
					{
					case TOKEN_CHINESE://������
					case TOKEN_WORD://word
					case TOKEN_SYMBOL://����
						CurWidth = GetStringWidth(token->p,token->len);
						if(SWInfo.mBoundary.right != 0 && SWInfo.mBoundary.right + CurWidth > TotalWidth)//��Ҫ����
						{
							if(bAdd)//�Ƚ���ǰ��
							{
								Line->mElements.push_back(SWInfo);	
								bAdd = false;
							}
							ADDLINE
						}
						if(!bAdd)//��ʼ��
						{
							SWInfo.Assign(token);
							SWInfo.AddWidth(CurWidth);
							SWInfo.mOffset = SWInfo.mStringID-strData;
							bAdd = true;
						}else
						{
							SWInfo.mCount		   += token->len;
							SWInfo.mBoundary.right += CurWidth;
						}
						break;
					case TOKEN_TAB://tab
						CurWidth = TABLENGHT * mFontWidth;
						if(bAdd)//�Ƚ���ǰ��
						{
							Line->mElements.push_back(SWInfo);	
							bAdd = false;
						}
						if(SWInfo.mBoundary.right != 0 && SWInfo.mBoundary.right + CurWidth > TotalWidth)//��Ҫ����
						{
							ADDLINE
						}
						SWInfo.Assign(token);
						SWInfo.AddWidth(CurWidth);
						SWInfo.mOffset = SWInfo.mStringID-strData;
						Line->mElements.push_back(SWInfo);	
						break;
					case TOKEN_ENTER://Enter
						{
							if(bAdd)//�Ƚ���ǰ��
							{
								Line->mElements.push_back(SWInfo);	
								bAdd = false;
							}
							SWInfo.Assign(token);
							SWInfo.AddWidth(mFontWidth);
							SWInfo.mOffset = SWInfo.mStringID-strData;
							Line->mElements.push_back(SWInfo);	
							ADDLINE
						}
						break;
					}
					token = WordLex.GetToken();
				}
				if(bAdd)//�����һ��Info��������
				{
					Line->mElements.push_back(SWInfo);	
				}
			}
			break;
		case CONTENT_PICTURE:
			{
				CurHeight	= jnItem->At(2).ToInt() + mVTextAxtra;
				CurWidth	= jnItem->At(1).ToInt();
				if(SWInfo.mBoundary.right != 0 && SWInfo.mBoundary.right + CurWidth > TotalWidth)//��Ҫ����
				{
					ADDLINE
				}
				if(MaxHeight<CurHeight)MaxHeight = CurHeight;
				SWInfo.SetPicture(CurWidth,CurHeight,jnItem);
				Line->mElements.push_back(SWInfo);				
			}
			break;
		case CONTENT_LINEICON:
			Line->mIconID = jnItem->At(0).ToCString();
			break;
		case CONTENT_TEXTATTRIBUTE:
			SetFont(jnItem);
			SWInfo.SetFont(SWInfo.mJnIndex,jnItem);
			if(MaxHeight<GetCurrentRowHeight())MaxHeight = GetCurrentRowHeight();
			break;
		}	
		SWInfo.mJnIndex++;
	}
	//���һ�����ݴ洢
	CardingHeight(Line,MaxHeight,SWInfo.mBoundary.right);
	mSize.width		+=  RESERVECONTENT;		
	mSize.height	+=  RESERVECONTENT;	
}

PMVoid PMEdit::CreateSystemSWInfos( TJSON *jnData,TJSON *jnSystemFont )
{
	//�����������	
	TTextWordLex WordLex;
	TTexToken   *token;
	ECElement    SWInfo  ;		
	TJSON	    *jnItem  ;
	//SIZE	     size;
	ECLine      *Line	    = CreateLine();
	PMInt	     CurHeight  = 0;
	PMInt	     CurWidth   = 0;
	PMInt	     jnSize	    = jnData->GetChildCount();
	PMInt	     MaxHeight  = GetCurrentRowHeight();	
	PMChar		*strData;
	mSize.clear();
	SWInfo.SetFont(-1,mJnSystemFont);
	CalcFontWidth();
	//�������е�����
	while(SWInfo.mJnIndex<jnSize)
	{		
		jnItem = &(jnData->At(SWInfo.mJnIndex));
		//2����ͼ��
		switch(GetType(jnItem))
		{
		case CONTENT_TEXT:
			{																
				strData = jnItem->ToCString();
				WordLex.SetData(strData);
				SWInfo.mBoundary.bottom  = GetCurrentRowHeight();
				SWInfo.mType		     = CONTENT_TEXT;
				token = WordLex.GetTokenEx();
				while (token->type)
				{
					switch(token->type)
					{
					case TOKEN_CONTENT://������
						SWInfo.Assign(token);
						SWInfo.AddWidth(GetStringWidth(token->p,token->len));
						SWInfo.mOffset = SWInfo.mStringID-strData;
						Line->mElements.push_back(SWInfo);	
						break;
					case TOKEN_TAB://tab
						SWInfo.Assign(token);
						SWInfo.AddWidth(TABLENGHT * mFontWidth);
						SWInfo.mOffset = SWInfo.mStringID-strData;
						Line->mElements.push_back(SWInfo);	
						break;
					case TOKEN_ENTER://Enter
						{
							SWInfo.Assign(token);
							SWInfo.AddWidth(mFontWidth);
							SWInfo.mOffset = SWInfo.mStringID-strData;
							Line->mElements.push_back(SWInfo);								
							CardingHeight(Line,MaxHeight,SWInfo.mBoundary.right);
							PMInt y		  = Line->mY + Line->mHeight;
							Line		  = CreateLine();
							Line->mY      = y;
							SWInfo.mBoundary.left  = 0;
							SWInfo.mBoundary.right = 0;
							MaxHeight	  = GetCurrentRowHeight();
						}
						break;
					}
					token = WordLex.GetTokenEx();
				}
			}
			break;
		case CONTENT_PICTURE:
			{
				CurHeight	= jnItem->At(2).ToInt() + mVTextAxtra;
				CurWidth	= jnItem->At(1).ToInt();
				if(MaxHeight<CurHeight)MaxHeight = CurHeight;
				SWInfo.SetPicture(CurWidth,CurHeight,jnItem);
				Line->mElements.push_back(SWInfo);				
			}
			break;
		case CONTENT_LINEICON:
			Line->mIconID = jnItem->At(0).ToCString();
			break;
		case CONTENT_TEXTATTRIBUTE:
			SetFont(jnItem);
			SWInfo.SetFont(SWInfo.mJnIndex,jnItem);
			if(MaxHeight<GetCurrentRowHeight())MaxHeight = GetCurrentRowHeight();
			break;
		}	
		SWInfo.mJnIndex++;
	}
	//���һ�����ݴ洢
	CardingHeight(Line,MaxHeight,SWInfo.mBoundary.right);
	mSize.width		+=  RESERVECONTENT;		
	mSize.height	+=  RESERVECONTENT;	
}

PMVoid PMEdit::CardingHeight( ECLine *Line,PMInt MaxHeight,PMInt MaxWidth )
{
	ECElements::iterator it = Line->mElements.begin();
	ECElements::iterator itEnd = Line->mElements.end();
	ECElement *element;	
	PMInt   y = Line->mY + MaxHeight ;
	for (; it != itEnd; it++)
	{
		element = &(*it);
		element->mBoundary.top    = y - element->mBoundary.bottom ;
		element->mBoundary.bottom = y;
		if(element->mType == CONTENT_PICTURE)
		{
			element->mIntID = mPictureManager->RegisterGifImage(element->mStringID);
		}
	}	
	if(Line->mIconID)
	{		
		Line->mGifID = mPictureManager->RegisterGifImage(Line->mIconID,true);
	}
	Line->mHeight = MaxHeight;
	Line->mWidth  = MaxWidth;
	if(mSize.width<MaxWidth)mSize.width = MaxWidth;
	mSize.height = Line->mY + MaxHeight;
}

PMVoid PMEdit::CreateCaretData()
{
	SetBeginPosition(mCaret.mLocation);
	mLastLD	= mCaret.mLocation ;
	AddPosition(GetPositionRect());
}

PMInt PMEdit::GetTotalWidth(PMInt	iLine)
{
	PMInt iWidth = mBoundary.width() - GetHeaderWidth(iLine) - mMargin.left - mMargin.right - RESERVEOFF;
	if(iWidth<mFontWidth)iWidth = mFontWidth;
	return iWidth;
}

PMInt PMEdit::GetTotalHeight()
{
	return mBoundary.height() - mMargin.top - mMargin.bottom;
}

PMInt PMEdit::GetHeaderWidth(PMInt	iLine)
{
	PMInt HeaderWidth = 0;
	if(mStyle&CONTENT_LINENUMBER)
	{
		HeaderWidth = GetLineNumberWidth(iLine);//�洢��������������
	}
	if(mStyle&CONTENT_LINEICON)
	{
		HeaderWidth += mIconWidth;//��������
	}
	return HeaderWidth + mMargin.left ;
}

PMInt PMEdit::GetType( TJSON *jnItem )
{
	
	if(jnItem->IsString())
	{
		return CONTENT_TEXT;
	}else if(jnItem->IsInt())
	{
		return CONTENT_TEXTATTRIBUTE;
	}else if(jnItem->IsArray())
	{
		switch(jnItem->GetChildCount())
		{
		case 1:return CONTENT_LINEICON;
		case 3:
		case 5:return CONTENT_PICTURE;
		case 7:return CONTENT_TEXTATTRIBUTE;
		}
	}
	return CONTENT_NONE;
}

PMVoid PMEdit::ShowText( ECLines::iterator &Line,ECElements::iterator &Element,PMInt State )
{	 
	ECElement &SWInfo = *Element;
	if(SWInfo.mJnFont!=mJnFont)SetFont(SWInfo.mJnFont);
	switch(SWInfo.mIntID)
	{
	case TOKEN_TAB://tab		
	case TOKEN_ENTER://Enter
		if (State==STATE_SELECTEDALL)
		{
			IntRect Bound = GetActualPosition(SWInfo.mBoundary);
			SetBrush(mTextSelectBKColor);
			FillRetangle(Bound);
		}
		break;
	case TOKEN_CONTENT://����
	case TOKEN_CHINESE://����
	case TOKEN_SYMBOL://����
	case TOKEN_WORD://����
		{			
			IntRect Bound   = GetActualPosition(SWInfo.mBoundary);
			PMInt   TextTop = Bound.top + mVTextAxtraOffset;
			PMColor txColor = GetTextColor(mCurrentHDC);
			PMChar *data	= SWInfo.GetString();
			PMInt	x		= Bound.left;
			switch(State)//-1��ѡ��0��ͨ��ȫ��1ѡ��2�༭
			{
			case STATE_MIDDLE:
				{
					//�Ȼ��Ʒ�ѡ�������
					TextOutA(mCurrentHDC, Bound.left,TextTop,data,mSelection.mFrom.mOffset);
					//�ٻ���ѡ�������
					SetBrush(mTextSelectBKColor);
					x += GetStringWidth(data,mSelection.mFrom.mOffset);
					Bound.left = x; 
					Bound.right = x + GetStringWidth(data+mSelection.mFrom.mOffset,mSelection.mTo.mOffset-mSelection.mFrom.mOffset);
					FillRetangle(Bound);
					SetTextColor(mCurrentHDC,mSelectedFormula->GetColor(txColor));								
					TextOutA(mCurrentHDC,x,TextTop,data+mSelection.mFrom.mOffset,mSelection.mTo.mOffset-mSelection.mFrom.mOffset);
					//�ָ���������δѡ������
					SetTextColor(mCurrentHDC,txColor);
					TextOutA(mCurrentHDC,Bound.right,TextTop,data+mSelection.mTo.mOffset,SWInfo.mCount-mSelection.mTo.mOffset);
				}
				break;
			case STATE_FRONT:
				{
					//�Ȼ��Ʒ�ѡ�������
					TextOutA(mCurrentHDC, Bound.left,TextTop,data,mSelection.mFrom.mOffset);
					//�ٻ���ѡ�������
					SetBrush(mTextSelectBKColor);
					x += GetStringWidth(data,mSelection.mFrom.mOffset);
					Bound.left = x; 
					FillRetangle(Bound);
					SetTextColor(mCurrentHDC,mSelectedFormula->GetColor(txColor));								
					TextOutA(mCurrentHDC, x,TextTop,data+mSelection.mFrom.mOffset,SWInfo.mCount-mSelection.mFrom.mOffset);
					//�ָ�����
					SetTextColor(mCurrentHDC,txColor);
				}
				break;
			case STATE_BACK:
				{										
					//�Ȼ���ѡ�������
					SetBrush(mTextSelectBKColor);
					x += GetStringWidth(data,mSelection.mTo.mOffset);
					Bound.right = x; 
					FillRetangle(Bound);
					SetTextColor(mCurrentHDC,mSelectedFormula->GetColor(txColor));		
					TextOutA(mCurrentHDC, Bound.left,TextTop,data,mSelection.mTo.mOffset);	
					//�ָ��������ٻ��Ʒ�ѡ�������
					SetTextColor(mCurrentHDC,txColor);
					TextOutA(mCurrentHDC, x,TextTop,data+mSelection.mTo.mOffset,SWInfo.mCount-mSelection.mTo.mOffset);						
				}
				break;
			case STATE_NORMAL:
				TextOutA(mCurrentHDC, Bound.left,TextTop,data,SWInfo.mCount);					
				break;
			case STATE_SELECTEDALL:
				{
					PMColor bkColor = GetBkColor(mCurrentHDC);
					PMColor txColor = GetTextColor(mCurrentHDC);
					SetBrush(mTextSelectBKColor);
					FillRetangle(Bound);
					SetTextColor(mCurrentHDC,mSelectedFormula->GetColor(txColor));		
					TextOutA(mCurrentHDC, Bound.left,TextTop,data,SWInfo.mCount);										
					SetTextColor(mCurrentHDC,txColor);
				}
				break;
			}
			SWInfo.Restore();
		}
		break;
	}
}

PMVoid PMEdit::ShowPicture( ECElement &SWInfo,PMInt State )
{
	IntRect Bound(SWInfo.mBoundary);
	Bound.top += mVTextAxtra;
	Bound = GetActualPosition(Bound);
	if(SWInfo.mIntID!=-1)
	{
		mPictureManager->Draw(mCurrentHDC,SWInfo.mIntID,Bound,State);	//0��ͨ״̬��1ѡ��״̬��2�༭״̬��	
	}else
	{
		mPictureManager->Draw(mCurrentHDC,SWInfo.mStringID,Bound,State);//0��ͨ״̬��1ѡ��״̬��2�༭״̬��	
	}
}

PMVoid PMEdit::ShowLineIcon( PMInt nLine)
{
	if(mStyle&CONTENT_LINEICON)
	{						
		ECLine *Line = mEDLines[nLine];
		PMInt LNWidth = GetLineNumberWidth(mVisibleLine);
		IntRect IconRect(LNWidth,0,LNWidth+mIconWidth,0);
		//��ʾ��һ��
		if(Line->mIconID)
		{
			IconRect.top	= Line->mY + mYOffset  ;
			IconRect.bottom = IconRect.top + Line->mHeight;
			if(Line->mGifID!=-1)
			{
				mPictureManager->DrawCenter(mCurrentHDC,Line->mGifID,IconRect);
			}else
			{
				mPictureManager->DrawCenter(mCurrentHDC,Line->mIconID,IconRect);
			}				
		}			
		//��ʾ������
		while ((++nLine)<(PMInt)mEDLines.size())
		{
			Line = mEDLines[nLine];
			if(IconRect.bottom < mBoundary.bottom)
			{
				if(Line->mIconID)
				{			
					IconRect.top	= Line->mY + mYOffset  ;
					IconRect.bottom = IconRect.top + Line->mHeight;
					if(Line->mGifID!=-1)
					{
						mPictureManager->DrawCenter(mCurrentHDC,Line->mGifID,IconRect);
					}else
					{
						mPictureManager->DrawCenter(mCurrentHDC,Line->mIconID,IconRect);
					}						
				}									
			}else
			{
				break;
			}
		}
	}
}

PMVoid PMEdit::ShowLineNumber( PMInt nLine )
{
	if(mStyle&CONTENT_LINENUMBER)
	{		
		PMInt   LNWidth = GetLineNumberWidth(mVisibleLine);
		PMInt	height  = mBoundary.height();
		IntRect LNBGRect(0,0,LNWidth,height);			
		SetFont(mLNTextAttribute);
		SetBrush(mLNBackColor);
		FillRetangle(LNBGRect);
		ECLine*  Line = mEDLines[nLine];
		IntRect LNRect;
		PMChar  buf[30] = {0};
		PMInt   len = 0;
		
		//��ʾ��һ��
		LNRect.left   = 0;
		LNRect.top    = Line->mY + mYOffset;
		LNRect.right  = LNWidth;
		LNRect.bottom = LNRect.top + Line->mHeight;
		itoa(nLine + 1,buf,10);
		len = strlen(buf);
		TextOutFormat(buf,len,LNRect);
		//��ʾ������
		while ((++nLine)<(PMInt)mEDLines.size())
		{
			Line = mEDLines[nLine];
			if(LNRect.bottom < height)
			{
				LNRect.top     = LNRect.bottom;
				LNRect.bottom += Line->mHeight;
				itoa(nLine+1,buf,10);
				len = strlen(buf);
				TextOutFormat(buf,len,LNRect);
			}else
			{
				break;
			}
		}
	}	
}

PMVoid PMEdit::Clear()
{
	mPictureManager->DeleteGifGroup(mImageGroup);
	for (ECLines::iterator it = mEDLines.begin();it != mEDLines.end(); it++)
	{
		delete (*it);
	}
	mEDLines.clear();	
}

PMEdit::ECLine* PMEdit::CreateLine()
{
	ECLine * newLineSTC = new ECLine;
	newLineSTC->mRow = mEDLines.size();
	mEDLines.push_back(newLineSTC);
	return newLineSTC;
}

PMVoid PMEdit::DeleteLine( PMInt iLine )
{
	delete mEDLines[iLine];
	mEDLines.erase(mEDLines.begin()+iLine);	
}

PMInt PMEdit::GetLineNumberWidth( PMInt iLine )
{
	PMInt count = 0;
	PMInt i = iLine;
	do
	{
		i=i/10;
		count++;
	}while (i);
	return iLine<100?DEFAULTLINENUMBERWIDTH + (count-1)*8:count*8;	
}

PMVoid PMEdit::GetInitData( TJSON *jnInitData )
{
	PMInt i  =0;
	mStyle			 = jnInitData->At(i++).ToInt();
	mbAutoLine		 = jnInitData->At(i++).ToBool();
	mbVScrollBar	 = jnInitData->At(i++).ToBool();
	mbHScrollBar	 = jnInitData->At(i++).ToBool();
	mbEdit			 = jnInitData->At(i++).ToBool();
	mBKColor		 = jnInitData->At(i++).ToInt();
	mLNBackColor	 = jnInitData->At(i++).ToInt();
	mLNTextAttribute = &jnInitData->At(i++);
	mTextSelectBKColor = jnInitData->At(i++).ToInt();
	mMargin			 = jnInitData->At(i++).GetRect();
	mVisibleX		 = jnInitData->At(i++).ToInt();
	mVisibleY		 = jnInitData->At(i++).ToInt();
	mVisibleLine	 = jnInitData->At(i++).ToInt() - 1;
	mIconDefaultWidth= jnInitData->At(i++).ToInt();
	mTextFormat		 = jnInitData->At(i++).ToInt();
	mHTextAxtra		 = jnInitData->At(i++).ToInt();
	mVTextAxtra		 = jnInitData->At(i++).ToInt();
	mVWheelSpeed	 = jnInitData->At(i++).ToInt();
	mHWheelSpeed	 = jnInitData->At(i++).ToInt();
	SetTextAxtra(mVTextAxtra,mHTextAxtra);
}

PMInt PMEdit::GetCurrentRowHeight()
{
	if(mStyle&CONTENT_LINEICON)return mIconWidth>mFontHeight?mIconWidth:mFontHeight;
	else return mFontHeight;
}

PMVoid PMEdit::GetIterStartLine()
{
	//ʹ�ö��ַ�����	
	if(!mEDLines.empty())
	{
		PMInt nLine;	
		PMInt i=0,j=mEDLines.size()-1;
		ECLine *Line = mEDLines[0];
		mVisibleLine = 0;
		if(mVisibleY<Line->mY+Line->mHeight)return;
		Line = mEDLines[j];
		if(mVisibleY>=Line->mY)
		{
			mVisibleLine = j;
			return;
		}		
		while (i<j)
		{
			nLine = (i+j)/2;
			Line  = mEDLines[nLine];
			if(mVisibleY >=Line->mY && Line->mY+Line->mHeight>mVisibleY)
			{
				mVisibleLine = nLine;
				break;
			}else if(mVisibleY<Line->mY)
			{
				j = nLine;
			}else if(Line->mY+Line->mHeight == mVisibleY)
			{
				mVisibleLine = nLine + 1;
				break;
			}else 
			{
				i = nLine;
			}
		}
	}
}


IntRect PMEdit::GetRetePosition( IntRect &Bound )
{
	IntRect Rect(Bound);
	Rect.left	-= mXOffset;
	Rect.right  -= mXOffset;
	Rect.top	-= mYOffset;
	Rect.bottom -= mYOffset;
	return Rect;	
}

IntPoint PMEdit::GetRetePosition( IntPoint &Position )
{
	IntPoint RetePosition(Position);
	RetePosition.x -= mXOffset;
	RetePosition.y -= mYOffset;
	return RetePosition;
}

IntPoint PMEdit::GetRetePosition( PMInt x,PMInt y )
{
	IntPoint RetePosition(x,y);
	RetePosition.x -= mXOffset;
	RetePosition.y -= mYOffset;
	return RetePosition;
}

IntRect PMEdit::GetActualPosition( IntRect &Bound )
{
	IntRect Rect(Bound);
	Rect.left	+= mXOffset;
	Rect.right  += mXOffset;
	Rect.top	+= mYOffset;
	Rect.bottom += mYOffset;
	return Rect;
}

IntPoint PMEdit::GetActualPosition( IntPoint &Position )
{
	IntPoint ActualPosition(Position);
	ActualPosition.x += mXOffset;
	ActualPosition.y += mYOffset;
	return ActualPosition;
}

IntPoint PMEdit::GetActualPosition( PMInt x,PMInt y )
{
	IntPoint ActualPosition(x,y);
	ActualPosition.x += mXOffset;
	ActualPosition.y += mYOffset;
	return ActualPosition;
}

PMEdit::Position PMEdit::GetPosition( TInputData *EventData )
{
	IntPoint	MousePoint = GetRetePosition(EventData->mMouseX,EventData->mMouseY);	
	Position  LD = GetPosition(MousePoint.x,MousePoint.y,EventData->mMouseX);	
	return LD;
}

PMEdit::Position PMEdit::GetOutsidePosition( PMInt x )
{
	Position  LD;	
	if(!mEDLines.empty())
	{
		ECLine *Line = mEDLines.front();		
		ECElement *content;
		ECElements::iterator iter	= Line->mElements.begin();
		ECElements::iterator iterEnd = Line->mElements.end();	
		for (; iter != iterEnd; iter++)
		{
			content = &(*iter);
			if (content->mBoundary.right >= x)
			{
				//4����ȡλ��
				LD.Assign(content);
				LD.mCol  = iter - Line->mElements.begin();
				LD.mRow  = Line->mRow;
				SetFont(content->mJnFont);
				LD.mFontHeight = mFontHeight;
				if(content->mType == CONTENT_TEXT)
				{
					GetPositionData(LD,content,x);//��Enter������ָ��ǰλ��					
				}else if(content->mType == CONTENT_PICTURE)
				{
					if(x>(content->mBoundary.right+content->mBoundary.left)/2)//���������ұ�
					{
						LD.mPositionX = content->mBoundary.right;
						LD.mOffset = 1;	
					}else
					{
						LD.mPositionX = content->mBoundary.left;
						LD.mOffset = 0;
					}	
				}
				break;
			}			
		}
		if(iter == iterEnd )
		{
			if(Line->mRow!=mEDLines.size()-1&&Line->mElements.size()>0)//��ĩβ�к���������
			{
				iter--;
				content = &(*iter);
				AssignPosition(LD,Line,content,iter - Line->mElements.begin(),content->mIntID == TOKEN_ENTER);
			}else
			{
				SetEndPosition(LD);
			}
		}
	}else
	{
		SetFont(mJnSystemFont);
		LD.mFontHeight = mFontHeight;
		LD.mPositionY  = mFontHeight;
		LD.mJnFont	   = mJnSystemFont;
	}
	return LD;
}

PMVoid PMEdit::AssignPosition( Position &LD,ECLine *Line,ECElement *content,PMInt iCol,PMBool bLeft )
{
	LD.Assign(content);
	SetFont(content->mJnFont);
	LD.mFontHeight = mFontHeight;
	LD.mRow  = Line->mRow;
	LD.mCol	 = iCol;
	if(bLeft)
	{
		LD.mOffset	   = 0;
		LD.mPositionX  = content->mBoundary.left;
	}else
	{
		LD.mOffset	   = content->mCount;
		LD.mPositionX  = content->mBoundary.right;
	}
}

PMEdit::Position PMEdit::GetPosition( PMInt x,PMInt y,PMInt MouseX )
{	
	if (MouseX<0&&y<=0)
	{
		return GetOutsidePosition(x);
	}
	PMBool		bCompleted = false;		
	Position  LD;
	//1����ȡ�к�
	ECElement   *content;
	ECLine      *Line ;	
	ECLines::iterator it	= mEDLines.begin();
	ECLines::iterator itEnd = mEDLines.end();	
	for(;it!=itEnd;it++)
	{
		Line = (*it);
		if(Line->mY > y)//����
		{
			if(Line->mElements.size()>0)
			{
				AssignPosition(LD,Line,&Line->mElements[0],0,true);
			}
			else
			{
				SetFont(mJnSystemFont);
				LD.mJnFont    = mJnSystemFont;
				LD.mPositionY = Line->mY + Line->mHeight;
				LD.mRow		  = Line->mRow;
			}
			bCompleted = true;
			break;
		}else if(Line->mY + Line->mHeight >= y)//�ҵ���������к�
		{
			LD.mPositionY = Line->mY + Line->mHeight;
			LD.mRow = Line->mRow;
			break;				
		}
	}
	if(it == itEnd)//���ݲ���Ĭ��Ϊ���һ��
	{
		SetEndPosition(LD);
		bCompleted = true;
	}
	//2����ȡ����
	if(MouseX >= 0)
	{
		PMInt LNWidth = GetLineNumberWidth(mVisibleLine);		//�кſ��
		PMInt XPos    = MouseX-mBoundary.left;  //��ǰλ��
		if(XPos < LNWidth + mIconWidth)
		{
			if(XPos >= LNWidth)LD.mSection = POSITION_LINEICON;//��ͼ��λ��
			else LD.mSection = POSITION_LINENUBER;			   //���к�λ��
			bCompleted = true;
		}
	}
	if(!bCompleted)
	{
		ECElements::iterator iter	 = Line->mElements.begin();
		ECElements::iterator iterEnd = Line->mElements.end();	
		for (; iter != iterEnd; iter++)
		{
			content = &(*iter);
			if (content->mBoundary.right >= x)
			{
				//4����ȡλ��
				LD.Assign(content);
				LD.mCol  = iter - Line->mElements.begin();
				LD.mRow  = Line->mRow;
				SetFont(content->mJnFont);
				LD.mFontHeight = mFontHeight;
				if(iter->mType == CONTENT_TEXT)
				{		
					GetPositionData(LD,content,x);//��Enter������ָ��ǰλ��						
				}else if(iter->mType == CONTENT_PICTURE)
				{
					if(x>(content->mBoundary.right+content->mBoundary.left)/2)//���������ұ�
					{
						LD.mPositionX = content->mBoundary.right;
						LD.mOffset = 1;	
					}else
					{
						LD.mPositionX = content->mBoundary.left;
						LD.mOffset = 0;
					}							
				}
				break;
			}					
		}
		if(iter == iterEnd)//���ݲ���Ĭ��Ϊ���һ��Ԫ��
		{
			if(Line->mRow!=mEDLines.size()-1&&Line->mElements.size()>0)//��ĩβ�к���������
			{
				iter--;
				content = &(*iter);
				AssignPosition(LD,Line,content,iter - Line->mElements.begin(),content->mIntID == TOKEN_ENTER);
			}else
			{
				SetEndPosition(LD);
			}
		}
	}
	if (LD.mPositionY==0)
	{
		LD.mPositionY=0;
	}
	return LD;
}

PMVoid PMEdit::GetPositionData( Position &LD,ECElement *content,PMInt x )
{
	switch(content->mIntID)
	{
	case 3://tab
		if( (content->mBoundary.left+content->mBoundary.right)/2 < x )
		{
			LD.mOffset    = 1;
			LD.mPositionX = content->mBoundary.right;
		}else
		{
			LD.mOffset     = 0;
			LD.mPositionX  = content->mBoundary.left;
		}
		break;
	case 4://Enter
		LD.mOffset    = 0;
		LD.mPositionX = content->mBoundary.left;
		break;		
	case 1://����
	case 2://����
	case 5://word
	case 6://content
		{
			if(x <= content->mBoundary.left)
			{
				LD.mOffset     = 0;
				LD.mPositionX  = content->mBoundary.left;
			}else if(x==content->mBoundary.right)
			{
				LD.mPositionX  = content->mBoundary.right;
				LD.mOffset     = content->mCount;
			}else//��ȡ����λ��
			{
				PMInt    dis   = x - content->mBoundary.left;
				PMChar * data  = content->GetString();
				PMInt	 width;
				PMInt    len      = 0;
				PMInt    CurWidth = 0;
				PMInt    Inc;
				while (CurWidth<dis)
				{
					if(data[len]<0)Inc = 2;
					else Inc = 1;
					width = GetStringWidth(data+len,Inc);
					if(CurWidth + width > dis)
					{
						if(CurWidth + width/2 <= dis)
						{
							len		 += Inc;
							CurWidth += width;
						}
						break;
					}
					len		  += Inc;
					CurWidth  += width;
				}
				content->Restore();
				LD.mPositionX  = content->mBoundary.left + CurWidth;				
				LD.mOffset     = len;
			}
		}
		break;
	}
}

PMBool PMEdit::IsMouseInSelection( TInputData *EventData )
{
	IntPoint pt = GetRetePosition(EventData->mMouseX,EventData->mMouseY);
	if(mSelection.mType!=0)
	{
		return mSelection.mTopBound.inside(pt)||mSelection.mMidBound.inside(pt)||mSelection.mBotBound.inside(pt);
	}
	return false;
}

PMInt PMEdit::GetContentState( ECElement *Element )
{
	PMInt Result = STATE_NORMAL;//-3�м�ѡ��,-2ǰ���ѡ��-1�����ѡ��0��ͨ��ȫ��1ѡ��2�༭
	switch(mSelection.mType)//-1ȫѡ��0û��ѡ��1��Χ��2ͼƬ�༭
	{
	case SELECTED_ALL:
		Result = STATE_SELECTEDALL;
		break;
	case SELECTED_SCOPE:
		{
			if(Element->mJnIndex==mSelection.mFrom.mJnIndex)
			{
				if(Element->mOffset==mSelection.mFrom.mOriOffset)
				{
					if(mSelection.mFrom.mOffset==0)
					{
						if(Element->mJnIndex==mSelection.mTo.mJnIndex)
						{
							if(Element->mOffset<mSelection.mTo.mOriOffset)
							{
								Result = STATE_SELECTEDALL;
							}else if(Element->mOffset==mSelection.mTo.mOriOffset)
							{
								if(mSelection.mTo.mOffset==Element->mCount)
								{
									Result = STATE_SELECTEDALL;
								}else if(mSelection.mTo.mOffset>0)
								{
									Result = STATE_BACK;
								}
							}
						}else if(Element->mJnIndex<mSelection.mTo.mJnIndex)
						{
							Result = STATE_SELECTEDALL;
						}
					}else if(mSelection.mFrom.mOffset<Element->mCount)
					{
						if(Element->mJnIndex==mSelection.mTo.mJnIndex)
						{
							if(Element->mOffset<mSelection.mTo.mOriOffset)
							{
								Result = STATE_FRONT;
							}else if(Element->mOffset==mSelection.mTo.mOriOffset)
							{
								if(mSelection.mTo.mOffset==Element->mCount)
								{
									Result = STATE_FRONT;
								}else if(mSelection.mTo.mOffset>0)
								{
									Result = STATE_MIDDLE;
								}
							}
						}else if(Element->mJnIndex<mSelection.mTo.mJnIndex)
						{
							Result = STATE_FRONT;
						}
					}
				}else if(Element->mOffset>mSelection.mFrom.mOriOffset)
				{
					if(Element->mJnIndex==mSelection.mTo.mJnIndex)
					{
						if(Element->mOffset<mSelection.mTo.mOriOffset)
						{
							Result = STATE_SELECTEDALL;
						}else if(Element->mOffset==mSelection.mTo.mOriOffset)
						{
							if(mSelection.mTo.mOffset==Element->mCount)
							{
								Result = STATE_SELECTEDALL;
							}else if(mSelection.mTo.mOffset>0)
							{
								Result = STATE_BACK;
							}
						}
					}else if(Element->mJnIndex<mSelection.mTo.mJnIndex)
					{
						Result = STATE_SELECTEDALL;
					}
				}
			}else if(Element->mJnIndex==mSelection.mTo.mJnIndex)
			{
				if(Element->mOffset<mSelection.mTo.mOriOffset)
				{
					Result = STATE_SELECTEDALL;
				}else if(Element->mOffset==mSelection.mTo.mOriOffset)
				{
					if(mSelection.mTo.mOffset==Element->mCount)
					{
						Result = STATE_SELECTEDALL;
					}else if(mSelection.mTo.mOffset>0)
					{
						Result = STATE_BACK;
					}
				}
			}else if(Element->mJnIndex>mSelection.mFrom.mJnIndex && Element->mJnIndex<mSelection.mTo.mJnIndex)
			{
				Result = STATE_SELECTEDALL;
			}
		}
		break;
	case SELECTED_PICTUREEDIT:
		if (Element->mJnIndex==mSelection.mFrom.mJnIndex)//ͼƬ�༭
		{
			Result = STATE_PICTUREEDIT;
		} 
		break;
	}
	return Result;
}


PMInt PMEdit::GetPositionX( Position &LD )
{
	ECElement *content = GetElement(LD);
	if (content)
	{
		if(LD.mOffset == 0)return content->mBoundary.left;
		if(LD.mOffset == content->mCount)return content->mBoundary.right;
		SetFont(LD.mJnFont);
		return content->mBoundary.left + GetStringWidth(content->mStringID,LD.mOffset);
	}
	return 0;
}

PMInt PMEdit::GetMouseArea( TInputData *EventData )
{	
	PMInt Result = 0;
	//1����ȡͼ����к���Ϣ
	PMInt LNWidth = GetLineNumberWidth(mVisibleLine);
	PMInt XPos    = EventData->mMouseX-mBoundary.left;
	if(XPos <= LNWidth + mIconWidth)
	{
		if(XPos >= LNWidth)Result = 2;//��ͼ��λ��
		else Result = 1;				//���к�λ��
	}else if(mSelection.mType!=0)
	{
		IntPoint pt = GetRetePosition(EventData->mMouseX,EventData->mMouseY);
		if( mSelection.mTopBound.inside(pt)||mSelection.mMidBound.inside(pt)||mSelection.mBotBound.inside(pt))Result = 3;
	}
	return Result;
}

PMVoid PMEdit::SetBeginPosition( Position &LD )
{	
	if (mEDLines.empty())
	{		
		LD.Clear();
		SetFont(mJnSystemFont);	
		LD.mJnFont     = mJnSystemFont;
		LD.mFontHeight = mFontHeight;
		LD.mPositionY  = mFontHeight;
	}else
	{
		ECLine *Line = mEDLines.front();
		if (Line->mElements.empty())//��һ��Ϊ�գ������ı���û�����ݣ�����������
		{
			LD.Clear();
			GetSystemFont(LD);
			LD.mJnIndex    = mJnData->GetChildCount();
			LD.mPositionY  = Line->mY + Line->mHeight;
			SetLastFont(LD,-1);
		}else
		{
			AssignPosition(LD,Line,&Line->mElements.front(),0,true);
		}
	}
}

PMVoid PMEdit::SetEndPosition( Position &LD )
{
	if (mEDLines.empty()||mEDLines.front()->mElements.empty())//û�����ݻ��ߵ�һ��Ϊ��
	{		
		LD.Clear();
		SetFont(mJnSystemFont);	
		LD.mJnFont     = mJnSystemFont;
		LD.mFontHeight = mFontHeight;
		LD.mPositionY  = mFontHeight;
		if(!mEDLines.empty())
		{
			ECLine *Line   = mEDLines.front();
			LD.mPositionY  = Line->mY + Line->mHeight;
			SetLastFont(LD,-1);
		}
	}	
	else
	{
		ECLine *Line = mEDLines.back();
		if (Line->mElements.empty())//���һ��Ϊ�գ���Ҫ��ȡ�ı�֮�������
		{
			LD.Clear();
			ECElement *Element = GetLastElement();
			LD.mPositionY = Line->mY + Line->mHeight;
			LD.mRow		  = Line->mRow;
			LD.mJnIndex   = mJnData->GetChildCount();
			LD.mJnFont    = Element->mJnFont;
			LD.mFontIndex = Element->mFontIndex;
			SetFont(LD.mJnFont);
			LD.mFontHeight = mFontHeight;
			SetLastFont(LD,Element->mJnIndex);
		}else
		{			
			ECElement *Element = &Line->mElements.back();
			AssignPosition(LD,Line,Element,Line->mElements.size()-1,false);
			SetLastFont(LD,Element->mJnIndex);//��ֹ�ı�ĩβ������������
		}
	}
}

PMVoid PMEdit::GetSystemFont( Position &LD )
{
	PMInt  i = mJnData->GetChildCount() - 1;
	TJSON *jnFont;
	LD.mJnFont    = mJnSystemFont;
	LD.mFontIndex = -1;
	while (i>=0)
	{
		jnFont = &mJnData->At(i);
		if (GetType(jnFont) == CONTENT_TEXTATTRIBUTE)
		{			
			LD.mJnFont    = jnFont;
			LD.mFontIndex = i;
			break;
		}
		i--;
	}
	SetFont(LD.mJnFont);
	LD.mFontHeight = mFontHeight;
}

PMVoid PMEdit::SetLastFont( Position &LD,PMInt Index )
{
	PMInt  i = mJnData->GetChildCount() - 1;
	TJSON *jnFont;
	while (i>Index)
	{
		jnFont = &mJnData->At(i);
		if (GetType(jnFont) == CONTENT_TEXTATTRIBUTE)
		{
			LD.mRow		  = mEDLines.size() - 1;
			LD.mRow		  = LD.mRow<0?0:LD.mRow;
			if(!mEDLines.empty())LD.mCol = mEDLines.back()->mElements.size();
			else LD.mCol  = 0;
			LD.mJnIndex   = mJnData->GetChildCount();
			LD.mOriOffset = 0;
			LD.mOffset	  = 0;
			LD.mJnFont    = jnFont;
			LD.mFontIndex = i;
			SetFont(LD.mJnFont);
			LD.mFontHeight = mFontHeight;
			break;
		}
		i--;
	}
}

PMVoid PMEdit::CalcSelectionBound()
{
	mSelection.mTopBound.clear();
	mSelection.mMidBound.clear();
	mSelection.mBotBound.clear();
	switch(mSelection.mType)
	{
	case SELECTED_ALL:
		mSelection.mTopBound.set(0,0,mSize.width,mSize.height);
		break;
	case SELECTED_NONE:
		return;
	case SELECTED_SCOPE:
		{
			if(mSelection.mFrom.mRow==mSelection.mTo.mRow)//�����
			{
				ECLine *Line = mEDLines[mSelection.mTo.mRow];
				mSelection.mTopBound.left   = GetPositionX(mSelection.mFrom);
				mSelection.mTopBound.top    = Line->mY;
				mSelection.mTopBound.right  = GetPositionX(mSelection.mTo);
				mSelection.mTopBound.bottom = Line->mY + Line->mHeight;
			}else
			{
				ECLine *LineFrom = mEDLines[mSelection.mFrom.mRow];
				ECLine *LineTo   = mEDLines[mSelection.mTo.mRow];
				mSelection.mTopBound.left   = GetPositionX(mSelection.mFrom);
				mSelection.mTopBound.top    = LineFrom->mY;
				mSelection.mTopBound.right  = LineFrom->mWidth;
				mSelection.mTopBound.bottom = mSelection.mTopBound.top + LineFrom->mHeight;
				if(mSelection.mTo.mRow-mSelection.mFrom.mRow>1)
				{
					mSelection.mBotBound.left   = 0;
					mSelection.mBotBound.top    = mSelection.mTopBound.bottom;
					mSelection.mBotBound.right  = mSize.width;
					mSelection.mBotBound.bottom = LineTo->mY;
				}
				mSelection.mBotBound.left   = 0;
				mSelection.mBotBound.top    = LineTo->mY;
				mSelection.mBotBound.right  = GetPositionX(mSelection.mTo);
				mSelection.mBotBound.bottom = mSelection.mBotBound.top + LineTo->mHeight;
			}			
		}
		break;
	case SELECTED_PICTUREEDIT:
		{
			ECElement *Element = GetElement(mSelection.mFrom);
			mSelection.mTopBound = Element->mBoundary;
		}
		break;
	}	
}

PMEdit::ECElement   * PMEdit::GetElement( Position &LD )
{
	ECElement *Result = NULL;
	if(LD.mSection == POSITION_RETE && LD.mRow < (PMInt)mEDLines.size())
	{
		if (LD.mCol<(PMInt)mEDLines[LD.mRow]->mElements.size())
		{
			Result = &mEDLines[LD.mRow]->mElements[LD.mCol];
		}
	}
	return Result;
}

PMEdit::ECElement   * PMEdit::GetLastElement()
{
	ECLines::reverse_iterator it = mEDLines.rbegin();
	if(it!=mEDLines.rend())
	{
		if(!(*it)->mElements.empty())return &(*it)->mElements.back();
		it++;
		if(it!=mEDLines.rend()&&!(*it)->mElements.empty())return &(*it)->mElements.back();
	}	
	return NULL;
}

PMEdit::ECElement   * PMEdit::GetPreElement( Position &LD )
{
	ECElement *Element = NULL;
	PMInt iRow = LD.mRow;
	PMInt iCol = LD.mCol;
	if(iRow>(PMInt)mEDLines.size())
	{		
		iRow = mEDLines.size() ;
		iCol = 0;
	}
	if (iCol==0)iRow--;
	if ( iRow>=0 && iRow < (PMInt)mEDLines.size())
	{
		ECLine *line = mEDLines[iRow];
		if (iCol == 0) iCol = (PMInt)line->mElements.size() -1;
		else iCol--;
		if (iCol>=0 && iCol<(PMInt)line->mElements.size())
		{
			Element = &line->mElements[iCol];
			LD.Assign(Element);
			SetFont(Element->mJnFont);
			LD.mFontHeight = mFontHeight;
			LD.mOffset	   = Element->mCount;
			LD.mPositionX  = Element->mBoundary.right;
			LD.mRow = iRow;
			LD.mCol = iCol;
		}
	}
	return Element;
}

PMEdit::ECElement   * PMEdit::GetNextElement( Position &LD )
{
	ECElement *Element = NULL;
	PMInt iRow = LD.mRow;
	PMInt iCol = LD.mCol;
	if ( iRow>=0 && iRow < (PMInt)mEDLines.size())
	{		
		ECLine *line = mEDLines[iRow];
		if (iCol == (PMInt)line->mElements.size() - 1) 
		{
			iRow++;
			iCol = 0;
			if(iRow>=(PMInt)mEDLines.size())return NULL;
			line = mEDLines[iRow];
		}else 
		{
			iCol++;
		}
		if (iCol>=0 && iCol<(PMInt)line->mElements.size())
		{
			Element = &line->mElements[iCol];
			LD.Assign(Element);
			SetFont(Element->mJnFont);
			LD.mFontHeight = mFontHeight;
			LD.mOffset	   = 0;
			LD.mPositionX  = Element->mBoundary.left;
			LD.mRow = iRow;
			LD.mCol = iCol;
		}
	}
	return Element;
}

PMVoid PMEdit::SetCaret( Position &LD,PMBool bSetLast,PMBool bSetX )
{
	HideCaret(mHdc);
	if(bSetX) mCaret.mLastX		  = LD.mPositionX;
	mCaret.mLocation			  = LD;
	if(bSetLast)mLastLD			  = LD;
	mCaret.mbState = true;
	DisplayCaret();
	PMInputManager::GetSingleton()->ResetCaretTimmer(mHWnd);
}

std::string PMEdit::GetText( TJSON *jnData )
{
	string Result;
	if(jnData)
	{
		TJSON::iterator it    = jnData->begin();
		TJSON::iterator itEnd = jnData->end();
		TJSON *jnItem;
		for (;it!=itEnd;it++)
		{
			jnItem =&(*it);
			if(GetType(jnItem)==CONTENT_TEXT)
			{
				Result += jnItem->ToCString();
			}
		}
	}	
	return Result;
}

std::string PMEdit::GetSelectedText()
{
	string Result;
	switch (mSelection.mType)
	{
	case SELECTED_ALL://ȫѡ	
		Result = GetText(mJnData);break;
	case SELECTED_SCOPE://��Χѡ��
		{
			ECElement *ElementFrom = GetElement(mSelection.mFrom);	 //����Ҫ��
			ECElement *ElementTo   = GetElement(mSelection.mTo);	 //����ΪNULL
			TJSON *jnItem;
			if (ElementFrom)
			{
				PMInt i,j;				
				i = ElementFrom->mJnIndex + 1;
				if(ElementTo)
				{
					j = ElementTo->mJnIndex;
					if (ElementFrom->mType == CONTENT_TEXT && ElementFrom->mJnIndex == ElementTo->mJnIndex)//ͬһ������
					{
						PMChar c = *(ElementTo->mStringID + mSelection.mTo.mOffset);
						*(ElementTo->mStringID + mSelection.mTo.mOffset) = 0;
						Result += ElementFrom->mStringID + mSelection.mFrom.mOffset;
						*(ElementTo->mStringID + mSelection.mTo.mOffset) = c;
					}else 
					{
						if(ElementFrom->mType == CONTENT_TEXT)
						{
							Result += ElementFrom->mStringID + mSelection.mFrom.mOffset;
						}
						while (i<j)
						{
							jnItem = &mJnData->At(i);
							if(GetType(jnItem)==CONTENT_TEXT)
							{
								Result += jnItem->ToCString();
							}
							i++;
						}
						if(ElementTo->mType == CONTENT_TEXT)
						{
							PMChar *data = ElementTo->mStringID - ElementTo->mOffset;
							PMChar c = *(ElementTo->mStringID + mSelection.mTo.mOffset);
							*(ElementTo->mStringID + mSelection.mTo.mOffset) = 0;
							Result += data;
							*(ElementTo->mStringID + mSelection.mTo.mOffset) = c;
						}
					}					
				}else
				{
					if (ElementFrom->mType == CONTENT_TEXT)
					{
						Result += ElementFrom->mStringID + mSelection.mFrom.mOffset;
					}
					j = mJnData->GetChildCount();
					while (i<j)
					{
						jnItem = &mJnData->At(i);
						if(GetType(jnItem)==CONTENT_TEXT)
						{
							Result += jnItem->ToCString();
						}
						i++;
					}
				}
			}
		}		
		break;
	}
	return Result;
}

PMVoid PMEdit::GetPack( TBuffer &pack )
{
	pack.Clear();
	TJSON jnPack;
	TJSON &jnPictures	  = jnPack[PICTURE_SECTION];
	TJSON &jnEditor		  = jnPack[EDCONTENT_SECTION];
	TJSON *jnFont		  = NULL;
	TJSON::iterator it    = mJnData->begin();
	TJSON::iterator itEnd = mJnData->end();
	TJSON *jnItem,*jnPicture;
	PMBool bText = false;
	string name;
	
	for (;it!=itEnd;it++)
	{
		jnItem = &(*it);
		switch(GetType(jnItem))
		{
		case CONTENT_PICTURE:
		case CONTENT_LINEICON:
			name	  = jnItem->At(0).ToString();			
			if(!jnPictures.Find(name))
			{
				jnPicture = mPictureManager->GetImageData(name);
				if(jnPicture)jnPictures.Insert(name,jnPicture);
			}
			break;
		case CONTENT_TEXT:
			bText = true;
			break;
		case CONTENT_TEXTATTRIBUTE:
			if(bText==false && jnFont == NULL)jnFont = jnItem;
			break;
		}
	}
	if (!jnFont)jnEditor.Insert(0,mJnSystemFont);
	jnEditor.RangeCopy(*mJnData,0);
	pack = jnPack.Print(false);
	jnPictures.RemoveAll();
	jnEditor.RemoveAll();
	jnPack.Clear();
}

PMVoid PMEdit::GetSelectedScopePack( TBuffer &pack )
{
	pack.Clear();
	ECElement *ElementFrom = GetElement(mSelection.mFrom);//����Ҫ��
	ECElement *ElementTo   = GetElement(mSelection.mTo);	 //����ΪNULL	
	if (ElementFrom)
	{
		TJSON jnPack;
		TJSON &jnPictures = jnPack[PICTURE_SECTION];
		TJSON &jnEditor   = jnPack[EDCONTENT_SECTION];		
		PMInt flag = 0;
		PMInt i,j;		
		TJSON *jnItem,*jnPicture;
		string name;
		i = ElementFrom->mJnIndex + 1;
		jnEditor.Push(mSelection.mFrom.mJnFont);
		if(ElementTo)
		{
			j = ElementTo->mJnIndex;
			if (ElementFrom->mType == CONTENT_TEXT && ElementFrom->mJnIndex == ElementTo->mJnIndex)//ͬһ������
			{				
				PMChar c = *(ElementTo->mStringID + mSelection.mTo.mOffset);
				*(ElementTo->mStringID + mSelection.mTo.mOffset) = 0;
				jnEditor.Push() = ElementFrom->mStringID + mSelection.mFrom.mOffset;
				*(ElementTo->mStringID + mSelection.mTo.mOffset) = c;
				flag = 1;
			}else 
			{
				if(ElementFrom->mType == CONTENT_TEXT)
				{
					if(*(ElementFrom->mStringID+mSelection.mFrom.mOffset)!=NULL)
					{
						jnEditor.Push() =  ElementFrom->mStringID+mSelection.mFrom.mOffset;
						flag = 1;
					}						
				}else if(mSelection.mFrom.mOffset!=1)//����������ͼƬʲô��
				{
					jnEditor.Push(&mJnData->At(ElementFrom->mJnIndex));
					name	  = ElementFrom->mStringID;			
					if(!jnPictures.Find(name))
					{
						jnPicture = mPictureManager->GetImageData(name);
						if(jnPicture)jnPictures.Insert(name,jnPicture);
					}					
				}
				if (i<j)
				{
					jnEditor.RangeCopy(*mJnData,i,j);
					TJSON::iterator it    = mJnData->begin() + i;
					TJSON::iterator itEnd = mJnData->begin() + j;						
					for (;it!=itEnd;it++)
					{
						jnItem = &(*it);
						switch(GetType(jnItem))
						{
						case CONTENT_PICTURE:
						case CONTENT_LINEICON:
							name	  = jnItem->At(0).ToString();			
							if(!jnPictures.Find(name))
							{
								jnPicture = mPictureManager->GetImageData(name);
								if(jnPicture)jnPictures.Insert(name,jnPicture);
							}
							break;
						}
					}
				}				
				if(ElementTo->mType == CONTENT_TEXT)
				{					
					if(ElementTo->mOffset+mSelection.mTo.mOffset>0)
					{		
						PMChar *data = ElementTo->mStringID - ElementTo->mOffset;
						PMChar c = *(ElementTo->mStringID + mSelection.mTo.mOffset);
						*(ElementTo->mStringID + mSelection.mTo.mOffset) = 0;
						jnEditor.Push() = data;
						*(ElementTo->mStringID + mSelection.mTo.mOffset) = c;
						if(flag == 1)flag = 3;
						else flag = 2;
					}						
				}else if(mSelection.mTo.mOffset!=0)//����������ͼƬʲô��
				{						
					jnEditor.Push(&mJnData->At(ElementTo->mJnIndex));
					name	  = ElementTo->mStringID;			
					if(!jnPictures.Find(name))
					{
						jnPicture = mPictureManager->GetImageData(name);
						if(jnPicture)jnPictures.Insert(name,jnPicture);
					}
				}
			}					
		}else
		{
			if (ElementFrom->mType == CONTENT_TEXT)
			{
				if(*(ElementFrom->mStringID+mSelection.mFrom.mOffset)!=NULL)
				{
					jnEditor.Push() = ElementFrom->mStringID + mSelection.mFrom.mOffset;
					flag = 1;
				}					
			}else if(mSelection.mFrom.mOffset!=1)//����������ͼƬʲô��
			{
				jnEditor.Push(&mJnData->At(ElementFrom->mJnIndex));
				name	  = ElementFrom->mStringID;			
				if(!jnPictures.Find(name))
				{
					jnPicture = mPictureManager->GetImageData(name);
					if(jnPicture)jnPictures.Insert(name,jnPicture);
				}				
			}
			j = mJnData->GetChildCount();
			if (i<j)
			{
				jnEditor.RangeCopy(*mJnData,i,j);
			}	
		}
		pack = jnPack.Print(false);
		if(flag == 3)
		{
			TJSON::DeleteJSON(&jnEditor.At(1));
			TJSON::DeleteJSON(&jnEditor.At(jnEditor.GetChildCount()-1));
		}else if(flag == 1)
		{
			TJSON::DeleteJSON(&jnEditor.At(1));
		}else if(flag == 2)
		{
			TJSON::DeleteJSON(&jnEditor.At(jnEditor.GetChildCount()-1));
		}
		jnPictures.RemoveAll();
		jnEditor.RemoveAll();
		jnPack.Clear();
	}
}


PMVoid PMEdit::GetPictureResizeArea(IntRect &ImageBound)
{
	const PMInt RADIUS  = 4;
	const PMInt ERADIUS = 5;
	mPictureResizeArea.mTop1.set(ImageBound.left-ERADIUS,ImageBound.top-ERADIUS,ImageBound.left+ERADIUS,ImageBound.top+ERADIUS);
	mPictureResizeArea.mTop3.set(ImageBound.right-ERADIUS,ImageBound.top-ERADIUS,ImageBound.right+ERADIUS,ImageBound.top+ERADIUS);
	mPictureResizeArea.mBottom1.set(ImageBound.left-ERADIUS,ImageBound.bottom-ERADIUS,ImageBound.left+ERADIUS,ImageBound.bottom+ERADIUS);
	mPictureResizeArea.mBottom3.set(ImageBound.right-ERADIUS,ImageBound.bottom-ERADIUS,ImageBound.right+ERADIUS,ImageBound.bottom+ERADIUS);
	PMInt THCent = (ImageBound.left + ImageBound.right)/2;
	PMInt BHCent = (ImageBound.top  + ImageBound.bottom)/2;
	mPictureResizeArea.mTop2.set(THCent-RADIUS,ImageBound.top-RADIUS,THCent+RADIUS,ImageBound.top+RADIUS);
	mPictureResizeArea.mBottom2.set(THCent-RADIUS,ImageBound.bottom-RADIUS,THCent+RADIUS,ImageBound.bottom+RADIUS);
	mPictureResizeArea.mMiddle1.set(ImageBound.left-RADIUS,BHCent-RADIUS,ImageBound.left+RADIUS,BHCent+RADIUS);
	mPictureResizeArea.mMiddle2.set(ImageBound.right-RADIUS,BHCent-RADIUS,ImageBound.right+RADIUS,BHCent+RADIUS);
	mPictureResizeArea.mNewBoundary = ImageBound;
	mPictureResizeArea.mBoundary = ImageBound;
}

PMVoid PMEdit::DrawPictureEditFrame()
{
	const PMInt RADIUS  = 2;
	RECT R;
	R.left   = mPictureResizeArea.mNewBoundary.left-RADIUS;
	R.top    = mPictureResizeArea.mNewBoundary.top-RADIUS;
	R.right  = mPictureResizeArea.mNewBoundary.right+RADIUS;
	R.bottom = mPictureResizeArea.mNewBoundary.top+RADIUS;
	InvertRect(mHdc,&R);
	R.left   = mPictureResizeArea.mNewBoundary.left-RADIUS;
	R.top    = mPictureResizeArea.mNewBoundary.bottom-RADIUS;
	R.right  = mPictureResizeArea.mNewBoundary.right+RADIUS;
	R.bottom = mPictureResizeArea.mNewBoundary.bottom+RADIUS;
	InvertRect(mHdc,&R);
	R.left   = mPictureResizeArea.mNewBoundary.left-RADIUS;
	R.top    = mPictureResizeArea.mNewBoundary.top+RADIUS;
	R.right  = mPictureResizeArea.mNewBoundary.left+RADIUS;
	R.bottom = mPictureResizeArea.mNewBoundary.bottom-RADIUS;
	InvertRect(mHdc,&R);
	R.left   = mPictureResizeArea.mNewBoundary.right-RADIUS;
	R.top    = mPictureResizeArea.mNewBoundary.top+RADIUS;
	R.right  = mPictureResizeArea.mNewBoundary.right+RADIUS;
	R.bottom = mPictureResizeArea.mNewBoundary.bottom-RADIUS;
	InvertRect(mHdc,&R);
}

IntRect PMEdit::GetPositionRect()
{
	return IntRect(mCaret.mLocation.mPositionX,mCaret.mLocation.mPositionY,mVisibleX,mVisibleY);
}

PMBool PMEdit::IsLastPosition( Position &LD )
{
	ECElement *Element = GetElement(LD);
	if(!Element)return true;
	if(LD.mRow!=mEDLines.size()-1)return false;
	if(LD.mCol!=mEDLines[LD.mRow]->mElements.size()-1)return false;
	if(LD.mOffset!=Element->mCount)return false;
	return true ;
}

PMBool PMEdit::IsBeginPosition( Position &LD )
{
	return LD.mRow == 0  && LD.mCol == 0 &&LD.mOffset == 0;
}

PMVoid PMEdit::SetScrollParameter()
{
	if(mbVScrollBar)
	{
		mScrollManager.mVScrollBar.SetMax(mSize.height);
		mScrollManager.mVScrollBar.SetStep(mFontHeight);
		mScrollManager.mVScrollBar.SetLargeStep(mVWheelSpeed);
		mScrollManager.mVScrollBar.SetPageValue(mContentHeight);
		mScrollManager.mVScrollBar.SetValue(mVisibleY);
		mScrollManager.mVScrollBar.LoadPicture();
	}
	if(mbHScrollBar)
	{
		mScrollManager.mHScrollBar.SetMax(mSize.width);
		mScrollManager.mHScrollBar.SetStep(mFontWidth);
		mScrollManager.mHScrollBar.SetLargeStep(mHWheelSpeed);
		mScrollManager.mHScrollBar.SetPageValue(mReteWidth);
		mScrollManager.mHScrollBar.SetValue(mVisibleX);
		mScrollManager.mHScrollBar.LoadPicture();
	}
}

PMVoid PMEdit::ShowScroll()
{
	if(mbVScrollBar)
	{
		if(mSize.height!=mScrollManager.mVScrollBar.GetMax())
		{
			mScrollManager.mVScrollBar.SetMax(mSize.height);
			mScrollManager.mVScrollBar.SetActualMax(mSize.height);
			mScrollManager.mVScrollBar.SetValue(mVisibleY);
			mScrollManager.mVScrollBar.ReCalcParameter(true);
		}else 
		{
			mScrollManager.mVScrollBar.SetValue(mVisibleY);
			mScrollManager.mVScrollBar.ReCalcParameter(false);
		}
	}
	if(mbHScrollBar)
	{
		if(mSize.width!=mScrollManager.mHScrollBar.GetMax())
		{
			mScrollManager.mHScrollBar.SetMax(mSize.width);
			mScrollManager.mHScrollBar.SetActualMax(mSize.width);
			mScrollManager.mHScrollBar.SetValue(mVisibleX);
			mScrollManager.mHScrollBar.ReCalcParameter(true);
		}else 
		{
			mScrollManager.mHScrollBar.SetValue(mVisibleX);
			mScrollManager.mHScrollBar.ReCalcParameter(false);
		}
	}
	if(mbHScrollBar&&mbVScrollBar)
	{
		mScrollManager.mNavigator.SetScope(mSize);
		mScrollManager.mNavigator.SetNavigator(IntRect(mVisibleX,mVisibleY,mVisibleX+mReteWidth,mVisibleY+mContentHeight));
	}
	mScrollManager.Show();
}

