
//
// BCGPRibbonSlider.cpp: implementation of the CBCGPRibbonSlider class.
//
//////////////////////////////////////////////////////////////////////

#include "BCGPRibbonSlider.h"
#include "BCGPVisualManager.h"
#include "BCGPRibbonBar.h"

#ifndef BCGP_EXCLUDE_RIBBON

IMPLEMENT_DYNCREATE(CBCGPRibbonSlider, CBCGPBaseRibbonElement)

static const int cxSliderWidth = 18;
static const int cySliderHeight = 18;

static const int cxThumbWidth = 10; 
static const int cyThumbHeight = 10; 

static const int nThumbIndex = 0;
static const int nSliderIndex = 1;
static const int nZoomInIndex = 2;
static const int nZoomOutIndex = 3;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPRibbonSlider::CBCGPRibbonSlider()
{
	CommonInit();
}
//****
CBCGPRibbonSlider::CBCGPRibbonSlider(
		UINT	nID, 
		int		nWidth,
		DWORD	dwStyle)
{
	CommonInit();

	m_nID = nID;
	m_nWidth = nWidth;
	m_dwStyle = dwStyle;
}
//****
CBCGPRibbonSlider::~CBCGPRibbonSlider()
{
}
//****
void CBCGPRibbonSlider::CommonInit()
{
	m_nMin = 0;
	m_nMax = 100;
	m_nPos = 0;
	m_nPageSize = 10;
	m_bZoomButtons = FALSE;
	m_nWidth = 100;
	m_dwStyle = 0;

	m_rectZoomOut.SetRectEmpty();
	m_rectZoomIn.SetRectEmpty();
	m_rectSlider.SetRectEmpty();
	m_rectThumb.SetRectEmpty();
	m_rectLabel.SetRectEmpty();

	m_nHighlighted = -1;
	m_nPressed = -1;
}
//****
void CBCGPRibbonSlider::OnDraw (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	if (m_rect.IsRectEmpty())
	{
		return;
	}

	if (m_bZoomButtons)
	{
		// Draw zoom buttons:
		CBCGPVisualManager::GetInstance()->OnDrawRibbonSliderZoomButton (
			pDC, this, m_rectZoomOut, TRUE, 
			m_bIsHighlighted && m_nHighlighted == nZoomOutIndex,
			m_bIsPressed && m_nPressed == nZoomOutIndex, IsDisabled());

		CBCGPVisualManager::GetInstance()->OnDrawRibbonSliderZoomButton (
			pDC, this, m_rectZoomIn, FALSE, 
			m_bIsHighlighted && m_nHighlighted == nZoomInIndex,
			m_bIsPressed && m_nPressed == nZoomInIndex, IsDisabled());
	}

	// Draw channel:
	CRect rectChannel = m_rectSlider;

	if (IsVert())
	{
		rectChannel.l = rectChannel.CenterPoint().x - 1;
		rectChannel.r = rectChannel.l + 2;
	}
	else
	{
		rectChannel.t = rectChannel.CenterPoint().y - 1;
		rectChannel.b = rectChannel.t + 2;
	}

	CBCGPVisualManager::GetInstance()->OnDrawRibbonSliderChannel (
		pDC, this, rectChannel);

	// Draw thumb:
	CBCGPVisualManager::GetInstance()->OnDrawRibbonSliderThumb (
		pDC, this, m_rectThumb,
		(m_bIsHighlighted && (m_nHighlighted == nThumbIndex || m_nHighlighted == nSliderIndex)) || IsFocused(),
		m_bIsPressed && m_nPressed == nThumbIndex, IsDisabled());

	if (!m_rectLabel.IsRectEmpty() && !m_strText.IsEmpty())
	{
		DoDrawText (pDC, m_strText, m_rectLabel, DT_CENTER | DT_SINGLELINE);
	}
}
//****
void CBCGPRibbonSlider::CopyFrom (const CBCGPBaseRibbonElement& s)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::CopyFrom (s);

	CBCGPRibbonSlider& src = (CBCGPRibbonSlider&) s;

	m_nMin = src.m_nMin;
	m_nMax = src.m_nMax;
	m_nPos = src.m_nPos;
	m_bZoomButtons = src.m_bZoomButtons;
	m_nWidth = src.m_nWidth;
	m_dwStyle = src.m_dwStyle;
}
//****
CSize CBCGPRibbonSlider::GetRegularSize (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	const BOOL bIsVert = IsVert();

	CSize size (bIsVert ? max (m_nWidth, cxSliderWidth) : m_nWidth, 
				bIsVert ? m_nWidth : cySliderHeight);

	if (IsQATMode() && (m_dwStyle & TBS_VERT))
	{
		size.w = 50;
	}

	double dblScale = globalData.GetRibbonImageScale();

	if (bIsVert)
	{
		if (!m_strText.IsEmpty())
		{
			int nTextWidth = pDC->GetTextExtent (m_strText).w;
			size.w = max (size.w, nTextWidth);
		}

		int nZoomButtonSize = cxSliderWidth;

		if (dblScale > 1.)
		{
			dblScale = 1. + (dblScale - 1.) / 2;
			size.w = (int)(.5 + dblScale * size.w);
			nZoomButtonSize = (int)(.5 + dblScale * nZoomButtonSize);
		}

		if (m_bZoomButtons)
		{
			size.h += 2 * nZoomButtonSize;
		}
	}
	else
	{
		if (dblScale > 1.)
		{
			dblScale = 1. + (dblScale - 1.) / 2;
			size.h = (int)(.5 + dblScale * size.h);
		}

		if (m_bZoomButtons)
		{
			size.w += 2 * size.h;
		}
	}

	return size;
}
//****
void CBCGPRibbonSlider::SetRange (int nMin, int nMax)
{
	ASSERT_VALID (this);

	m_nMin = nMin;
	m_nMax = nMax;
}
//****
void CBCGPRibbonSlider::SetPos (int nPos, BOOL bRedraw)
{
	ASSERT_VALID (this);

	m_nPos = min (max (m_nMin, nPos), m_nMax);
	SetThumbRect();

	if (!m_bDontNotify)
	{
		CBCGPRibbonBar* pRibbonBar = GetTopLevelRibbonBar();
		if (pRibbonBar != NULL)
		{
			ASSERT_VALID (pRibbonBar);

			CArray<CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*> arButtons;
			pRibbonBar->GetElementsByID (m_nID, arButtons);

			for (int i = 0; i < arButtons.GetSize(); i++)
			{
				CBCGPRibbonSlider* pOther =
					DYNAMIC_DOWNCAST (CBCGPRibbonSlider, arButtons [i]);

				if (pOther != NULL && pOther != this)
				{
					ASSERT_VALID (pOther);

					pOther->m_bDontNotify = TRUE;
					pOther->SetPos (nPos);
					pOther->m_bDontNotify = FALSE;
				}
			}
		}
	}

	if (bRedraw)
	{
		Redraw();
	}
}
//****
void CBCGPRibbonSlider::SetPageSize (int nPageSize)
{
	ASSERT_VALID (this);
	m_nPageSize = nPageSize;
}
//****
void CBCGPRibbonSlider::SetZoomButtons (BOOL bSet)
{
	ASSERT_VALID (this);
	m_bZoomButtons = bSet;
}
//****
void CBCGPRibbonSlider::OnAfterChangeRect (CDC* pDC)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::OnAfterChangeRect (pDC);

	m_rectZoomOut.SetRectEmpty();
	m_rectZoomIn.SetRectEmpty();
	m_rectThumb.SetRectEmpty();
	m_rectLabel.SetRectEmpty();

	m_rectSlider = m_rect;

	if (m_rect.IsRectEmpty())
	{
		return;
	}

	if (IsVert() && !m_strText.IsEmpty())
	{
		m_rectLabel = m_rectSlider;
		m_rectSlider.b -= globalData.GetTextHeight();
		m_rectLabel.t = m_rectSlider.b;
	}

	if (m_bZoomButtons)
	{
		m_rectZoomOut = m_rectSlider;

		if (IsVert())
		{
			m_rectZoomOut.b = m_rectZoomOut.t + cySliderHeight;
		}
		else
		{
			m_rectZoomOut.r = m_rectZoomOut.l + m_rectZoomOut.Height();
		}

		m_rectZoomIn = m_rectSlider;

		if (IsVert())
		{
			m_rectZoomIn.t = m_rectZoomIn.b - cySliderHeight;

			m_rectSlider.t = m_rectZoomOut.b;
			m_rectSlider.b = m_rectZoomIn.t;
		}
		else
		{
			m_rectZoomIn.l = m_rectZoomIn.r - m_rectZoomIn.Height();

			m_rectSlider.l = m_rectZoomOut.r;
			m_rectSlider.r = m_rectZoomIn.l;
		}
	}

	int nThumbSize = IsVert() ? cyThumbHeight : cxThumbWidth;
	
	double dblScale = globalData.GetRibbonImageScale();

	if (dblScale > 1.)
	{
		dblScale = 1. + (dblScale - 1.) / 2;
		nThumbSize = (int)(.5 + dblScale * nThumbSize);
	}

	if (IsVert())
	{
		m_rectSlider.DeflateRect (0, nThumbSize / 2);
	}
	else
	{
		m_rectSlider.DeflateRect (nThumbSize / 2, 0);
	}

	SetThumbRect();
}
//****
void CBCGPRibbonSlider::SetThumbRect()
{
	ASSERT_VALID (this);

	if (m_nMax <= m_nMin || m_rect.IsRectEmpty())
	{
		m_rectThumb.SetRectEmpty();
		return;
	}

	m_rectThumb = m_rectSlider;

	double dblScale = globalData.GetRibbonImageScale();

	if (IsVert())
	{
		double dy = ((double) m_rectSlider.Height()) / (m_nMax - m_nMin);
		int yOffset = (int) (.5 + dy * (m_nPos - m_nMin));

		int nThumbHeight = cyThumbHeight;

		if (m_rectThumb.Width() > cxSliderWidth)
		{
			m_rectThumb.l = m_rectThumb.CenterPoint().x - cxSliderWidth / 2;
			m_rectThumb.r = m_rectThumb.l + cxSliderWidth;
		}
		
		if (dblScale > 1.)
		{
			dblScale = 1. + (dblScale - 1.) / 2;

			nThumbHeight = (int)(.5 + dblScale * nThumbHeight);
			m_rectThumb.DeflateRect (4, 0);
		}

		m_rectThumb.b -= yOffset - nThumbHeight / 2;
		m_rectThumb.t = m_rectThumb.b - nThumbHeight;
	}
	else
	{
		double dx = ((double) m_rectSlider.Width()) / (m_nMax - m_nMin);
		int xOffset = (int) (.5 + dx * (m_nPos - m_nMin));

		int nThumbWidth = cxThumbWidth;
		
		if (dblScale > 1.)
		{
			dblScale = 1. + (dblScale - 1.) / 2;

			nThumbWidth = (int)(.5 + dblScale * nThumbWidth);
			m_rectThumb.DeflateRect (0, 4);
		}

		m_rectThumb.l += xOffset - nThumbWidth / 2;
		m_rectThumb.r = m_rectThumb.l + nThumbWidth;
	}
}
//****
void CBCGPRibbonSlider::OnLButtonDown (CPoint point)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::OnLButtonDown (point);

	if (!IsDisabled())
	{
		m_nPressed = GetHitTest (point);
		Redraw();
	}
}
//****
void CBCGPRibbonSlider::OnLButtonUp (CPoint point)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::OnLButtonUp (point);

	if (m_nPressed == m_nHighlighted && !IsDisabled())
	{
		switch (m_nPressed)
		{
		case nZoomInIndex:
			SetPos (m_nPos + m_nPageSize);
			break;

		case nZoomOutIndex:
			SetPos (m_nPos - m_nPageSize);
			break;

		default:
			SetPos (GetPosFromPoint (point));
			break;
		}

		NotifyCommand();
	}
}
//****
void CBCGPRibbonSlider::OnMouseMove (CPoint point)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::OnMouseMove (point);

	if (IsDisabled())
	{
		return;
	}

	int nHighlightedOld = m_nHighlighted;

	m_nHighlighted = GetHitTest (point);

	if (nHighlightedOld != m_nHighlighted)
	{
		Redraw();
	}

	if (m_bIsPressed)
	{
		if (m_nPressed == nThumbIndex)
		{
			int nPos = GetPosFromPoint (point);
			if (nPos != m_nPos)
			{
				SetPos (nPos);
				NotifyCommand();
			}
		}
	}
}
//****
int CBCGPRibbonSlider::GetHitTest (CPoint point) const
{
	ASSERT_VALID (this);

	if (m_rectThumb.PtInRect (point))
	{
		return nThumbIndex;
	}

	if (m_rectSlider.PtInRect (point))
	{
		return nSliderIndex;
	}

	if (m_rectZoomOut.PtInRect (point))
	{
		return nZoomOutIndex;
	}

	if (m_rectZoomIn.PtInRect (point))
	{
		return nZoomInIndex;
	}

	return -1;
}
//****
int CBCGPRibbonSlider::GetPosFromPoint (CPoint pt)
{
	ASSERT_VALID (this);

	if (m_nMax <= m_nMin || m_rect.IsRectEmpty())
	{
		return m_nMin;
	}

	if (IsVert())
	{
		const double dy = ((double) m_rectSlider.Height()) / (m_nMax - m_nMin);
		const int yOffset = m_rectSlider.b - pt.y;

		return m_nMin + (int) ((double) yOffset / dy);
	}
	else
	{
		const double dx = ((double) m_rectSlider.Width()) / (m_nMax - m_nMin);
		const int xOffset = pt.x - m_rectSlider.l;

		return m_nMin + (int) ((double) xOffset / dx);
	}
}
//****
BOOL CBCGPRibbonSlider::IsAutoRepeatMode (int& /*nDelay*/) const
{
	ASSERT_VALID (this);
	return m_nPressed == nZoomInIndex || m_nPressed == nZoomOutIndex;
}
//****
BOOL CBCGPRibbonSlider::OnAutoRepeat()
{
	ASSERT_VALID (this);

	if (m_bIsDisabled)
	{
		return FALSE;
	}

	if (m_nPressed == nZoomInIndex)
	{
		SetPos (m_nPos + m_nPageSize);
		NotifyCommand();
	}
	else if (m_nPressed == nZoomOutIndex)
	{
		SetPos (m_nPos - m_nPageSize);
		NotifyCommand();
	}

	return TRUE;
}
//****
void CBCGPRibbonSlider::NotifyCommand()
{
	ASSERT_VALID (this);

	if (m_nID == 0 || m_nID == (UINT)-1)
	{
		return;
	}

	CBCGPRibbonBar* pRibbonBar = GetTopLevelRibbonBar();
	if (pRibbonBar == NULL)
	{
		return;
	}

	ASSERT_VALID (pRibbonBar);

	CWnd* pWndParent = pRibbonBar->GetParent();
	if (pWndParent == NULL)
	{
		return;
	}

	pWndParent->SendMessage (WM_COMMAND, m_nID);
}
//****
void CBCGPRibbonSlider::OnDrawOnList (CDC* pDC, CString strText,
									  int nTextOffset, CRect rect,
									  BOOL /*bIsSelected*/,
									  BOOL /*bHighlighted*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	const int nSliderWidth = IsVert() ? (cxSliderWidth + 2) : (rect.Height() * 2);

	BOOL bIsDisabled = m_bIsDisabled;
	m_bIsDisabled = FALSE;

	CRect rectText = rect;

	rectText.l += nTextOffset;
	rectText.r -= nSliderWidth;

	const int xMargin = 3;
	rectText.DeflateRect (xMargin, 0);

	pDC->DrawText (strText, rectText, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

	// Draw channel:
	CRect rectChannel = rect;
	rectChannel.l = rectChannel.r - nSliderWidth;

	if (IsVert())
	{
		rectChannel.l = rectChannel.CenterPoint().x - 1;
		rectChannel.r = rectChannel.l + 2;
	}
	else
	{
		rectChannel.t = rectChannel.CenterPoint().y - 1;
		rectChannel.b = rectChannel.t + 2;
	}

	CBCGPVisualManager::GetInstance()->OnDrawRibbonSliderChannel (
		pDC, this, rectChannel);

	// Draw thumb:
	CRect rectThumb = rect;
	rectThumb.l = rectThumb.r - nSliderWidth;

	if (IsVert())
	{
		rectThumb.t = rectThumb.CenterPoint().y - 2;
		rectThumb.b = rectThumb.CenterPoint().y + 3;
		rectThumb.DeflateRect (1, 0);
	}
	else
	{
		rectThumb.l = rectThumb.CenterPoint().x - 2;
		rectThumb.r = rectThumb.CenterPoint().x + 2;
		rectThumb.DeflateRect (0, 1);
	}

	CBCGPVisualManager::GetInstance()->OnDrawRibbonSliderThumb (
		pDC, this, rectThumb,
		FALSE, FALSE, FALSE);

	m_bIsDisabled = bIsDisabled;
}
//****
BOOL CBCGPRibbonSlider::OnProcessKey (UINT nChar)
{
	ASSERT_VALID (this);

	const int nPrevPos = m_nPos;
	int nPos = m_nPos;

	switch (nChar)
	{
	case VK_LEFT:
		if (!IsVert())
		{
			nPos--;
		}
		else
		{
			return FALSE;
		}
		break;

	case VK_UP:
		if (IsVert())
		{
			nPos--;
		}
		else
		{
			return FALSE;
		}
		break;

	case VK_PRIOR:
		nPos -= m_nPageSize;
		break;

	case VK_NEXT:
		nPos += m_nPageSize;
		break;

	case VK_RIGHT:
		if (!IsVert())
		{
			nPos++;
		}
		else
		{
			return FALSE;
		}
		break;

	case VK_DOWN:
		if (IsVert())
		{
			nPos++;
		}
		else
		{
			return FALSE;
		}
		break;

	case VK_HOME:
		nPos = m_nMin;
		break;

	case VK_END:
		nPos = m_nMax;
		break;

	default:
		return FALSE;
	}

	nPos = min (max (m_nMin, nPos), m_nMax);

	if (nPrevPos != nPos)
	{
		SetPos (nPos);
		NotifyCommand();
	}

	return TRUE;
}

#endif // BCGP_EXCLUDE_RIBBON
