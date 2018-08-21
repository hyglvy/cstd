#include "StdAfx.h"
#include "PMEdit.h"



PMBool PMEdit::OnSize( TInputData *EventData )
{
	if(EventData->mSizeWidth == 0 || EventData->mSizeHeight == 0)return true;
	IntRect BRect(0,0,EventData->mSizeWidth,EventData->mSizeHeight);
	if (BRect!=mBoundary)
	{
		PMInt scrollType = SCROLL_NONE;
		if(mbHScrollBar)scrollType  = SCROLL_HORIZON;
		if(mbVScrollBar)scrollType |= SCROLL_VERTICAL;
		mBoundary = BRect;		
		mScrollManager.Initialize(this,mHWnd,mBoundary,&mProperty->At(LAFERS),scrollType);	
		WindowResize(mBoundary);
		Refresh(false);
		Show(false,false);
		SetScrollParameter();
		OnPaint(EventData);
	}
	return true;
}

PMBool PMEdit::OnPaint( TInputData *EventData )
{
	DrawFrame(&mBoundary);
	DisplayCaret();
	ShowScroll();
	return true;
}

PMBool PMEdit::OnGifTimmer( TInputData *EventData )
{
	SelectClipRgn(mCurrentHDC,mhRgnReteRect);
	if(mPictureManager->TimerEvent(mImageGroup,mCurrentHDC))
	{		
		DrawFrame(&mBoundary);
		DisplayCaret();
		if(mMouseState == PMEDITOR_MOUSE_PICTURERESIZE)DrawPictureEditFrame();
	}
	SelectClipRgn(mCurrentHDC,NULL);
	return true;
}

PMBool PMEdit::OnLButtonDown( TInputData *EventData )
{	
	mMouseState = PMEDITOR_MOUSE_DEFAULT;
	if(InPictureResizeArea(EventData->mMouseX,EventData->mMouseY))
	{
		mMouseState = PMEDITOR_MOUSE_PICTURERESIZE;
		mPictureResizeArea;
		DrawPictureEditFrame();
	}else if(IsMouseInSelection(EventData))
	{
		Position LD = GetPosition(EventData);
		OnContentDrag(LD);
	}else
	{		
		Position LD = GetPosition(EventData);
		switch(LD.mSection)//��ǰ�ķ���-2�к�λ�á�-1ͼ��λ�á�0����λ��
		{
		case POSITION_LINENUBER://������к�
			OnLNAreaClick(LD,EventData);
			break;
		case POSITION_LINEICON://�����ͼ��
			OnIconAreaClick(LD);
			break;
		case POSITION_RETE://���������
			SetCursor(LoadCursor(NULL, IDC_IBEAM));	
			OnReteAreaClick(LD,EventData);
			break;
		}
	}	
	return true;
}


PMBool PMEdit::OnSetFocus( TInputData *EventData )
{
	mCaret.mVisible = true;
	DisplayCaret();
	return true;
}

PMBool PMEdit::OnLostFocus( TInputData *EventData )
{
	//mCaret.mVisible = false;
	//HideCaret(mHdc);
	return true;
}

PMBool PMEdit::OnMouseDrag( TInputData *EventData )
{
	return true;
}

PMBool PMEdit::OnDragOver( TInputData *EventData )
{
	return true;
}

PMBool PMEdit::OnMouseDrop( TInputData *EventData )
{
	return true;
}

PMBool PMEdit::OnMouseMove( TInputData *EventData )
{
	if(mReteRect.inside(EventData->mMouseX,EventData->mMouseY))
	{
		switch(mMouseState)
		{
		case PMEDITOR_MOUSE_PICTUREEDIT:
			{
				Position LD =GetPosition(EventData);
				if(LD.mSection==POSITION_RETE)
				{
					OnSelectionAdd(LD);	
					Show(true);
				}
			}
			break;
		case PMEDITOR_MOUSE_SELECTED:
			{
				Position LD =GetPosition(EventData);
				if(LD.mSection==POSITION_RETE)
				{
					if (mLastLD!=LD)
					{
						OnSelectionAdd(LD);					
					}else
					{
						mSelection.Clear();
						SetCaret(LD);
					}	
					Show(true);
				}		
			}
			break;
		case PMEDITOR_MOUSE_PICTURERESIZE:
			{
				OnPictureResize(EventData);
			}
			break;
		case PMEDITOR_MOUSE_LINESELECT:
			OnLineSelected(EventData);
			break;
		case PMEDITOR_MOUSE_BEGINDRAG:		
			break;
		case PMEDITOR_MOUSE_DRAG:
			SetCursor(LoadCursor(NULL, IDC_UPARROW));	
			break;
		default:
			switch(GetMouseArea(EventData))
			{
			case 0://���±༭����
				if(!InPictureResizeArea(EventData->mMouseX,EventData->mMouseY))
				{
					SetCursor(LoadCursor(NULL, IDC_IBEAM));	
				}			
				break;
			case 1://�к�����
				SetCursor(LoadCursor(NULL, IDC_ARROW));	
				break;
			case 2://��ͼ������
				SetCursor(LoadCursor(NULL, IDC_ARROW));	
				break;
			case 3://����ѡ������
				if(!InPictureResizeArea(EventData->mMouseX,EventData->mMouseY))
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));	
				}
				break;
			}
			break;
		}
	}else
	{
		OnMouseMoveOutside(EventData);
	}
	return true;
}

PMVoid PMEdit::OnMouseMoveOutside( TInputData *EventData )
{
	switch(mMouseState)
	{
		case PMEDITOR_MOUSE_SELECTED:
			{				
				OnMouseSelectOutside(EventData);
			}
			break;
		case PMEDITOR_MOUSE_PICTURERESIZE:
			{				
				OnPictureResize(EventData);
			}
			break;
		case PMEDITOR_MOUSE_LINESELECT:
			{
				OnLineSelected(EventData);
			}
			break;
	}
}

PMBool PMEdit::InPictureResizeArea( PMInt x,PMInt y )
{
	if (mSelection.mType == SELECTED_PICTUREEDIT)//��ͼƬ�༭״̬
	{
		if(mPictureResizeArea.mTop1.inside(x,y))
		{
			mPictureResizeArea.mIndex = 1;
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));	
			return true;
		}else if(mPictureResizeArea.mBottom3.inside(x,y))
		{
			mPictureResizeArea.mIndex = 8;
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));	
			return true;
		}else if(mPictureResizeArea.mTop3.inside(x,y))
		{
			mPictureResizeArea.mIndex = 3;
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));	
			return true;
		}else if(mPictureResizeArea.mBottom1.inside(x,y))
		{
			mPictureResizeArea.mIndex = 6;
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));	
			return true;
		}else if(mPictureResizeArea.mMiddle1.inside(x,y))
		{
			mPictureResizeArea.mIndex = 4;
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));	
			return true;
		}else if(mPictureResizeArea.mMiddle2.inside(x,y))
		{
			mPictureResizeArea.mIndex = 5;
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));	
			return true;
		}else if(mPictureResizeArea.mTop2.inside(x,y))
		{
			mPictureResizeArea.mIndex = 2;
			SetCursor(LoadCursor(NULL, IDC_SIZENS));	
			return true;
		}else if(mPictureResizeArea.mBottom2.inside(x,y))
		{
			mPictureResizeArea.mIndex = 7;
			SetCursor(LoadCursor(NULL, IDC_SIZENS));	
			return true;
		}
	}
	return false;
}

PMBool PMEdit::OnMouseWheel( TInputData *EventData )
{
	if (EventData->IsShiftPress())//�����ҹ���
	{		
		OnHScrollView((-EventData->mMouseZ)*mHWheelSpeed);
	}else//�����¹���
	{
		OnVScrollView((-EventData->mMouseZ)*mVWheelSpeed);
	}	
	return true;
}

PMBool PMEdit::OnLButtonDBLCLK( TInputData *EventData )
{
	OnLButtonDown(EventData);
	OnSelectedIdentical();
	return true;
}

PMBool PMEdit::OnRButtonDown( TInputData *EventData )
{

	return true;
}

PMBool PMEdit::OnLButtonUp( TInputData *EventData )
{
	switch(mMouseState)
	{
	case PMEDITOR_MOUSE_DEFAULT:
	case PMEDITOR_MOUSE_SELECTED:
		AddPosition(GetPositionRect());
		//if(mSelection.mType == -1||mSelection.mType == 1)mSelection.Clear();
		break;
	case PMEDITOR_MOUSE_BEGINDRAG:
		{
			Position LD = GetPosition(EventData);
			OnReteAreaClick(LD,EventData);
		}
		break;
	case PMEDITOR_MOUSE_PICTURERESIZE:
		OnPictureResizeCompleted(EventData);
		break;
	}
	mMouseState = PMEDITOR_MOUSE_DEFAULT;
	return true;
}

PMBool PMEdit::OnRButtonUp( TInputData *EventData )
{
	return true;
}

PMBool PMEdit::OnKeyDown( TInputData *EventData )
{
	if (EventData->IsControlPress())
	{
		switch(EventData->mKey[0])
		{
		case 'A':DoSelectAll();	return true;//ȫѡ
		case 'X':CommandCut();	return true;//����
		case 'C':DoCopy();		return true;//����
		case 'V':CommandPaste();return true;//ճ��
		case 'Z':UnDo();		return true;//����
		case 'Y':ReDo();		return true;//����
		case 'S':DoSave();		return true;//����
		case TInputData::VLK_BACK:   ControlBack();return true;//ɾ��
		case TInputData::VLK_DELETE: ControlDelete();return true;//ɾ��
		case TInputData::VLK_HOME:   ControlHome();	 return true;//�ص����¿�ͷ
		case TInputData::VLK_END:    ControlEnd();	 return true;//�ص����½�β
		}
	}
	if(EventData->IsShiftPress())
	{
		switch(EventData->mKey[0])
		{
		case TInputData::VLK_DOWN:   ShiftDown();	 return true;//��         
		case TInputData::VLK_UP:     ShiftUp();		 return true;//��
		case TInputData::VLK_RIGHT:  ShiftRight();	 return true;//��
		case TInputData::VLK_LEFT:   ShiftLeft();	 return true;//��
		case TInputData::VLK_NEXT:   ShiftPageDown();return true;//��һҳ
		case TInputData::VLK_PRIOR:  ShiftPageUp();	 return true;//��һҳ
		case TInputData::VLK_HOME:   ShiftHome();	 return true;//��ͷ
		case TInputData::VLK_END:    ShiftEnd();	 return true;//��β
		}
	}
	if(EventData->IsAltPress())
	{
		switch(EventData->mKey[0])
		{
		case TInputData::VLK_RIGHT:  AltRight();	 return true;//�½���
		case TInputData::VLK_LEFT:   AltLeft();		 return true;//�Ͻ���
		}
	}
	switch(EventData->mKey[0])
	{
	case TInputData::VLK_DELETE: CommandDelete();	break;//ɾ��
	case TInputData::VLK_ESCAPE: DoEscape();		break;//����
	case TInputData::VLK_DOWN:   DoDown();			break;//��         
	case TInputData::VLK_UP:     DoUp();			break;//��
	case TInputData::VLK_RIGHT:  DoRight();			break;//��
	case TInputData::VLK_LEFT:   DoLeft();			break;//��
	case TInputData::VLK_NEXT:   DoPageDown();		break;//��һҳ
	case TInputData::VLK_PRIOR:  DoPageUp();		break;//��һҳ
	case TInputData::VLK_HOME:   DoHome();			break;//��ͷ
	case TInputData::VLK_END:    DoEnd();			break;//��β
	case TInputData::VLK_INSERT: DoInsert();		break;//����
	}
	return true;
}

PMBool PMEdit::OnKeyUp( TInputData *EventData )
{
	return true;
}

PMBool PMEdit::OnChar( TInputData *EventData )
{
	if (!(EventData->IsControlPress()||EventData->IsAltPress()))
	{
		OnKeyChar(EventData);
	}	
	return true;
}

PMBool PMEdit::OnIMEChar( TInputData *EventData )
{
	OnKeyChar(EventData);
	return true;
}

PMBool PMEdit::OnDropFiles( TInputData *EventData )
{
	return true;
}

PMBool PMEdit::OnCaretTimmer( TInputData *EventData )
{			
	DrawCaret();					
	return true;
}

PMBool PMEdit::OnEnter( TInputData *EventData )
{
	return true;
}

PMBool PMEdit::OnLeave( TInputData *EventData )
{
	return true;
}

PMVoid PMEdit::OnHScroll( PMInt iRange )
{
	OnHScrollView(iRange);	
}

PMVoid PMEdit::OnVScroll( PMInt iRange )
{
	OnVScrollView(iRange);
}

PMVoid PMEdit::OnNavigate( IntRect &Navigator )
{
	PMBool bChangeX = HScrollView(Navigator.left-mVisibleX);
	PMBool bChangeY = VScrollView(Navigator.top-mVisibleY);
	if(bChangeX||bChangeY)Show(true);
}