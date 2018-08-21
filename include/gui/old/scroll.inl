/* data for a single scroll bar */
typedef struct {
  INT curVal; /* Current scroll-bar value */
  INT minVal; /* Minimum scroll-bar value */
  INT maxVal; /* Maximum scroll-bar value */
  INT page; /* Page size of scroll bar (Win32) */
  UINT flags; /* EnableScrollBar flags */
} SCROLLBAR_INFO, *LPSCROLLBAR_INFO;
/* data for window that has (one or two) scroll bars */
typedef struct {
  SCROLLBAR_INFO horz;
  SCROLLBAR_INFO vert;
} WINSCROLLBAR_INFO, *LPWINSCROLLBAR_INFO;
/* Minimum size of the rectangle between the arrows */
#define SCROLL_MIN_RECT 4
/* Minimum size of the thumb in pixels */
#define SCROLL_MIN_THUMB 6
/* Overlap between arrows and thumb */
#define SCROLL_ARROW_THUMB_OVERLAP 0
/* Delay (in ms) before first repetition when holding the button down */
#define SCROLL_FIRST_DELAY 200
/* Delay (in ms) between scroll repetitions */
#define SCROLL_REPEAT_DELAY 50
/* Scroll timer id */
#define SCROLL_TIMER 0
/* Scroll-bar hit testing */
enum SCROLL_HITTEST {
  SCROLL_NOWHERE, /* Outside the scroll bar */
  SCROLL_TOP_ARROW, /* Top or left arrow */
  SCROLL_TOP_RECT, /* Rectangle between the top arrow and the thumb */
  SCROLL_THUMB, /* Thumb rectangle */
  SCROLL_BOTTOM_RECT, /* Rectangle between the thumb and the bottom arrow */
  SCROLL_BOTTOM_ARROW /* Bottom or right arrow */
};
/* What to do after SCROLL_SetScrollInfo() */
#define SA_SSI_HIDE 0x0001
#define SA_SSI_SHOW 0x0002
#define SA_SSI_REFRESH 0x0004
#define SA_SSI_REPAINT_ARROWS 0x0008
/* Thumb-tracking info */
static HWID SCROLL_TrackingWin = 0;
static INT SCROLL_TrackingBar = 0;
static INT SCROLL_TrackingPos = 0;
static INT SCROLL_TrackingVal = 0;
/* Hit test code of the last button-down event */
static enum SCROLL_HITTEST SCROLL_trackHitTest;
static BOOL SCROLL_trackVertical;
/* Is the moving thumb being displayed? */
static BOOL SCROLL_MovingThumb = FALSE;
/* Local functions */
static BOOL SCROLL_ShowScrollBar(HWID hWid, INT nBar, BOOL fShowH, BOOL fShowV);
static INT SCROLL_SetScrollInfo(HWID hWid, INT nBar, const SCROLLINFO* info, BOOL bRedraw);
static void SCROLL_DrawInterior_9x(HWID hWid, HDC hdc, INT nBar,
    RECT* rect, INT arrowSize,
    INT thumbSize, INT thumbPos,
    UINT flags, BOOL vertical,
    BOOL top_selected, BOOL bottom_selected);
/*********************************************************************
 * scrollbar class descriptor
 */
static const char scrollbarW[] = "ScrollBar";
#if 0
const struct builtin_class_descr SCROLL_builtin_class = {
  scrollbarW, /* name */
  CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW | CS_PARENTDC, /* style */
  WINPROC_SCROLLBAR, /* proc */
  sizeof(SCROLLBAR_INFO), /* extra */
  IDC_ARROW, /* cursor */
  0 /* brush */
};
#endif
/***********************************************************************
 * SCROLL_ScrollInfoValid
 *
 * Determine if the supplied SCROLLINFO struct is valid.
 * info [in] The SCROLLINFO struct to be tested
 */
static inline BOOL SCROLL_ScrollInfoValid(LPCSCROLLINFO info)
{
  return !(info->fMask & ~(SIF_ALL | SIF_DISABLENOSCROLL)
      || (info->cbSize != sizeof(*info)
          && info->cbSize != sizeof(*info) - sizeof(info->nTrackPos)));
}
/***********************************************************************
 * SCROLL_GetInternalInfo
 * Returns pointer to internal SCROLLBAR_INFO structure for nBar
 * or NULL if failed (f.i. scroll bar does not exist yet)
 * If alloc is TRUE and the struct does not exist yet, create it.
 */
static SCROLLBAR_INFO* SCROLL_GetInternalInfo(HWID hWid, INT nBar, BOOL alloc)
{
  SCROLLBAR_INFO* infoPtr = NULL;
  HWID wndPtr = hWid;
  switch (nBar) {
  case SB_HORZ:
    if (wndPtr->pScroll) {
      infoPtr = &((LPWINSCROLLBAR_INFO)wndPtr->pScroll)->horz;
    }
    break;
  case SB_VERT:
    if (wndPtr->pScroll) {
      infoPtr = &((LPWINSCROLLBAR_INFO)wndPtr->pScroll)->vert;
    }
    break;
  case SB_CTL:
    infoPtr = (SCROLLBAR_INFO*)wndPtr->wExtra;
    break;
  case SB_BOTH:
    WARN("with SB_BOTH\n");
    break;
  }
  if (!infoPtr && alloc) {
    WINSCROLLBAR_INFO* winInfoPtr;
    if (nBar != SB_HORZ && nBar != SB_VERT) {
      WARN("Cannot initialize nBar=%d\n", nBar);
    }
    else if ((winInfoPtr = (WINSCROLLBAR_INFO*)HeapAlloc(GetProcessHeap(), 0, sizeof(WINSCROLLBAR_INFO)))) {
      /* Set default values */
      winInfoPtr->horz.minVal = 0;
      winInfoPtr->horz.curVal = 0;
      winInfoPtr->horz.page = 0;
      /* From MSDN and our own tests:
      * max for a standard scroll bar is 100 by default. */
      winInfoPtr->horz.maxVal = 100;
      winInfoPtr->horz.flags = ESB_ENABLE_BOTH;
      winInfoPtr->vert = winInfoPtr->horz;
      wndPtr->pScroll = winInfoPtr;
      infoPtr = nBar == SB_HORZ ? &winInfoPtr->horz : &winInfoPtr->vert;
    }
  }
  //WIN_ReleasePtr( wndPtr );
  return infoPtr;
}
/***********************************************************************
 * SCROLL_GetScrollBarRect
 *
 * Compute the scroll bar rectangle, in drawing coordinates (i.e. client
 * coords for SB_CTL, window coords for SB_VERT and SB_HORZ).
 * 'arrowSize' returns the width or height of an arrow (depending on
 * the orientation of the scrollbar), 'thumbSize' returns the size of
 * the thumb, and 'thumbPos' returns the position of the thumb
 * relative to the left or to the top.
 * Return TRUE if the scrollbar is vertical, FALSE if horizontal.
 */
static BOOL SCROLL_GetScrollBarRect(HWID hWid, INT nBar, RECT* lprect,
    INT* arrowSize, INT* thumbSize,
    INT* thumbPos)
{
  INT pixels;
  BOOL vertical;
  HWID wndPtr = (hWid);
  switch (nBar) {
  case SB_HORZ:
    GetWidgetRect(hWid, lprect);
    lprect->top = lprect->bottom;
    lprect->bottom += GetSystemMetrics(SM_CYHSCROLL);
    if (wndPtr->dwStyle & WS_VSCROLL) {
      lprect->right++;
    }
    vertical = FALSE;
    break;
  case SB_VERT:
    GetWidgetRect(hWid, lprect);
    if ((wndPtr->dwExStyle & WS_EX_LEFTSCROLLBAR) != 0) {
      lprect->right = lprect->left;
      lprect->left -= GetSystemMetrics(SM_CXVSCROLL);
    }
    else {
      lprect->left = lprect->right;
      lprect->right += GetSystemMetrics(SM_CXVSCROLL);
    }
    if (wndPtr->dwStyle & WS_HSCROLL) {
      lprect->bottom++;
    }
    vertical = TRUE;
    break;
  case SB_CTL:
    GetWidgetRect(hWid, lprect);
    vertical = ((wndPtr->dwStyle & SBS_VERT) != 0);
    break;
  default:
    WIN_ReleasePtr(wndPtr);
    return FALSE;
  }
  if (vertical) {
    pixels = lprect->bottom - lprect->top;
  }
  else {
    pixels = lprect->right - lprect->left;
  }
  if (pixels <= 2 * GetSystemMetrics(SM_CXVSCROLL) + SCROLL_MIN_RECT) {
    if (pixels > SCROLL_MIN_RECT) {
      *arrowSize = (pixels - SCROLL_MIN_RECT) / 2;
    }
    else {
      *arrowSize = 0;
    }
    *thumbPos = *thumbSize = 0;
  }
  else {
    SCROLLBAR_INFO* info = SCROLL_GetInternalInfo(hWid, nBar, FALSE);
    if (!info) {
      WARN("called for missing scroll bar\n");
      WIN_ReleasePtr(wndPtr);
      return FALSE;
    }
    *arrowSize = GetSystemMetrics(SM_CXVSCROLL);
    pixels -= (2 * (GetSystemMetrics(SM_CXVSCROLL) - SCROLL_ARROW_THUMB_OVERLAP));
    if (info->page) {
      *thumbSize = MulDiv(pixels, info->page, (info->maxVal - info->minVal + 1));
      if (*thumbSize < SCROLL_MIN_THUMB) {
        *thumbSize = SCROLL_MIN_THUMB;
      }
    }
    else {
      *thumbSize = GetSystemMetrics(SM_CXVSCROLL);
    }
    if (((pixels -= *thumbSize) < 0) ||
        ((info->flags & ESB_DISABLE_BOTH) == ESB_DISABLE_BOTH)) {
      /* Rectangle too small or scrollbar disabled -> no thumb */
      *thumbPos = *thumbSize = 0;
    }
    else {
      INT max = info->maxVal - max(info->page - 1, 0);
      if (info->minVal >= max) {
        *thumbPos = *arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
      }
      else
        *thumbPos = *arrowSize - SCROLL_ARROW_THUMB_OVERLAP
            + MulDiv(pixels, (info->curVal - info->minVal), (max - info->minVal));
    }
  }
  WIN_ReleasePtr(wndPtr);
  return vertical;
}
/***********************************************************************
 * SCROLL_GetThumbVal
 *
 * Compute the current scroll position based on the thumb position in pixels
 * from the top of the scroll-bar.
 */
static UINT SCROLL_GetThumbVal(SCROLLBAR_INFO* infoPtr, RECT* rect,
    BOOL vertical, INT pos)
{
  INT thumbSize;
  INT pixels = vertical ? rect->bottom - rect->top : rect->right - rect->left;
  INT range;
  if ((pixels -= 2 * (GetSystemMetrics(SM_CXVSCROLL) - SCROLL_ARROW_THUMB_OVERLAP)) <= 0) {
    return infoPtr->minVal;
  }
  if (infoPtr->page) {
    thumbSize = MulDiv(pixels, infoPtr->page, (infoPtr->maxVal - infoPtr->minVal + 1));
    if (thumbSize < SCROLL_MIN_THUMB) {
      thumbSize = SCROLL_MIN_THUMB;
    }
  }
  else {
    thumbSize = GetSystemMetrics(SM_CXVSCROLL);
  }
  if ((pixels -= thumbSize) <= 0) {
    return infoPtr->minVal;
  }
  pos = max(0, pos - (GetSystemMetrics(SM_CXVSCROLL) - SCROLL_ARROW_THUMB_OVERLAP));
  if (pos > pixels) {
    pos = pixels;
  }
  if (!infoPtr->page) {
    range = infoPtr->maxVal - infoPtr->minVal;
  }
  else {
    range = infoPtr->maxVal - infoPtr->minVal - infoPtr->page + 1;
  }
  return infoPtr->minVal + MulDiv(pos, range, pixels);
}
/***********************************************************************
 * SCROLL_PtInRectEx
 */
static BOOL SCROLL_PtInRectEx(LPRECT lpRect, POINT pt, BOOL vertical)
{
  RECT rect = *lpRect;
  int scrollbarWidth;
  /* Pad hit rect to allow mouse to be dragged outside of scrollbar and
  * still be considered in the scrollbar. */
  if (vertical) {
    scrollbarWidth = lpRect->right - lpRect->left;
    rect.left -= scrollbarWidth * 8;
    rect.right += scrollbarWidth * 8;
    rect.top -= scrollbarWidth * 2;
    rect.bottom += scrollbarWidth * 2;
  }
  else {
    scrollbarWidth = lpRect->bottom - lpRect->top;
    rect.left -= scrollbarWidth * 2;
    rect.right += scrollbarWidth * 2;
    rect.top -= scrollbarWidth * 8;
    rect.bottom += scrollbarWidth * 8;
  }
  return PtInRect(&rect, pt);
}
/***********************************************************************
 * SCROLL_ClipPos
 */
static POINT SCROLL_ClipPos(LPRECT lpRect, POINT pt)
{
  if (pt.x < lpRect->left) {
    pt.x = lpRect->left;
  }
  else if (pt.x > lpRect->right) {
    pt.x = lpRect->right;
  }
  if (pt.y < lpRect->top) {
    pt.y = lpRect->top;
  }
  else if (pt.y > lpRect->bottom) {
    pt.y = lpRect->bottom;
  }
  return pt;
}
/***********************************************************************
 * SCROLL_HitTest
 *
 * Scroll-bar hit testing (don't confuse this with WM_NCHITTEST!).
 */
static enum SCROLL_HITTEST SCROLL_HitTest(HWID hWid, INT nBar,
    POINT pt, BOOL bDragging)
{
  INT arrowSize, thumbSize, thumbPos;
  RECT rect;
  BOOL vertical = SCROLL_GetScrollBarRect(hWid, nBar, &rect,
      &arrowSize, &thumbSize, &thumbPos);
  BOOL asdf = PtInRect(&rect, pt);
  if ((bDragging && !SCROLL_PtInRectEx(&rect, pt, vertical)) ||
      (!PtInRect(&rect, pt))) {
    return SCROLL_NOWHERE;
  }
  if (vertical) {
    if (pt.y < rect.top + arrowSize) {
      return SCROLL_TOP_ARROW;
    }
    if (pt.y >= rect.bottom - arrowSize) {
      return SCROLL_BOTTOM_ARROW;
    }
    if (!thumbPos) {
      return SCROLL_TOP_RECT;
    }
    pt.y -= rect.top;
    if (pt.y < thumbPos) {
      return SCROLL_TOP_RECT;
    }
    if (pt.y >= thumbPos + thumbSize) {
      return SCROLL_BOTTOM_RECT;
    }
  }
  else { /* horizontal */
    if (pt.x < rect.left + arrowSize) {
      return SCROLL_TOP_ARROW;
    }
    if (pt.x >= rect.right - arrowSize) {
      return SCROLL_BOTTOM_ARROW;
    }
    if (!thumbPos) {
      return SCROLL_TOP_RECT;
    }
    pt.x -= rect.left;
    if (pt.x < thumbPos) {
      return SCROLL_TOP_RECT;
    }
    if (pt.x >= thumbPos + thumbSize) {
      return SCROLL_BOTTOM_RECT;
    }
  }
  return SCROLL_THUMB;
}
/***********************************************************************
 * SCROLL_DrawArrows
 *
 * Draw the scroll bar arrows.
 */
static void SCROLL_DrawArrows(HDC hdc, SCROLLBAR_INFO* infoPtr,
    RECT* rect, INT arrowSize, BOOL vertical,
    BOOL top_pressed, BOOL bottom_pressed)
{
  RECT r;
  r = *rect;
  if (vertical) {
    r.bottom = r.top + arrowSize;
  }
  else {
    r.right = r.left + arrowSize;
  }
  DrawFrameControl(hdc, &r, DFC_SCROLL,
      (vertical ? DFCS_SCROLLUP : DFCS_SCROLLLEFT)
      | (top_pressed ? (DFCS_PUSHED | DFCS_FLAT) : 0)
      | (infoPtr->flags & ESB_DISABLE_LTUP ? DFCS_INACTIVE : 0));
  r = *rect;
  if (vertical) {
    r.top = r.bottom - arrowSize;
  }
  else {
    r.left = r.right - arrowSize;
  }
  DrawFrameControl(hdc, &r, DFC_SCROLL,
      (vertical ? DFCS_SCROLLDOWN : DFCS_SCROLLRIGHT)
      | (bottom_pressed ? (DFCS_PUSHED | DFCS_FLAT) : 0)
      | (infoPtr->flags & ESB_DISABLE_RTDN ? DFCS_INACTIVE : 0));
}
static void SCROLL_DrawMovingThumb(HDC hdc, RECT* rect, BOOL vertical,
    INT arrowSize, INT thumbSize)
{
  INT pos = SCROLL_TrackingPos;
  INT max_size;
  if (vertical) {
    max_size = rect->bottom - rect->top;
  }
  else {
    max_size = rect->right - rect->left;
  }
  max_size -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP) + thumbSize;
  if (pos < (arrowSize - SCROLL_ARROW_THUMB_OVERLAP)) {
    pos = (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
  }
  else if (pos > max_size) {
    pos = max_size;
  }
  SCROLL_DrawInterior_9x(SCROLL_TrackingWin, hdc, SCROLL_TrackingBar, rect, arrowSize, thumbSize, pos, 0, vertical, FALSE, FALSE);
  SCROLL_MovingThumb = !SCROLL_MovingThumb;
}
/***********************************************************************
 * SCROLL_DrawInterior
 *
 * Draw the scroll bar interior (everything except the arrows).
 */
static void SCROLL_DrawInterior_9x(HWID hWid, HDC hdc, INT nBar,
    RECT* rect, INT arrowSize,
    INT thumbSize, INT thumbPos,
    UINT flags, BOOL vertical,
    BOOL top_selected, BOOL bottom_selected)
{
  RECT r;
  HPEN hSavePen;
  HBRUSH hSaveBrush, hBrush;
  /* Only scrollbar controls send WM_CTLCOLORSCROLLBAR.
  * The window-owned scrollbars need to call DEFWND_ControlColor
  * to correctly setup default scrollbar colors
  */
  if (nBar == SB_CTL) {
    hBrush = (HBRUSH)SendMessageW(GetParent(hWid), WM_CTLCOLORSCROLLBAR,
        (WPARAM)hdc, (LPARAM)hWid);
  }
  else {
    hBrush = DEFWND_ControlColor(hdc, CTLCOLOR_SCROLLBAR);
  }
  hSavePen = (HPEN)SelectObject(hdc, SYSCOLOR_GetPen(COLOR_WINDOWFRAME));
  hSaveBrush = (HBRUSH)SelectObject(hdc, hBrush);
  /* Calculate the scroll rectangle */
  r = *rect;
  if (vertical) {
    r.top += arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
    r.bottom -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
  }
  else {
    r.left += arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
    r.right -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
  }
  /* Draw the scroll rectangles and thumb */
  if (!thumbPos) { /* No thumb to draw */
    PatBlt(hdc, r.left, r.top,
        r.right - r.left, r.bottom - r.top,
        PATCOPY);
    /* cleanup and return */
    SelectObject(hdc, hSavePen);
    SelectObject(hdc, hSaveBrush);
    return;
  }
  if (vertical) {
    PatBlt(hdc, r.left, r.top,
        r.right - r.left,
        thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP),
        top_selected ? 0x0f0000 : PATCOPY);
    r.top += thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
    PatBlt(hdc, r.left, r.top + thumbSize,
        r.right - r.left,
        r.bottom - r.top - thumbSize,
        bottom_selected ? 0x0f0000 : PATCOPY);
    r.bottom = r.top + thumbSize;
  }
  else { /* horizontal */
    PatBlt(hdc, r.left, r.top,
        thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP),
        r.bottom - r.top,
        top_selected ? 0x0f0000 : PATCOPY);
    r.left += thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
    PatBlt(hdc, r.left + thumbSize, r.top,
        r.right - r.left - thumbSize,
        r.bottom - r.top,
        bottom_selected ? 0x0f0000 : PATCOPY);
    r.right = r.left + thumbSize;
  }
  /* Draw the thumb */
  DrawEdge(hdc, &r, EDGE_RAISED, BF_RECT | BF_MIDDLE);
  /* cleanup */
  SelectObject(hdc, hSavePen);
  SelectObject(hdc, hSaveBrush);
}
static void SCROLL_DrawInterior(HWID hWid, HDC hdc, INT nBar,
    RECT* rect, INT arrowSize,
    INT thumbSize, INT thumbPos,
    UINT flags, BOOL vertical,
    BOOL top_selected, BOOL bottom_selected)
{
  RECT r;
  HPEN hSavePen;
  HBRUSH hSaveBrush, hBrush;
  BOOL Save_SCROLL_MovingThumb = SCROLL_MovingThumb;
  if (Save_SCROLL_MovingThumb &&
      (SCROLL_TrackingWin == hWid) &&
      (SCROLL_TrackingBar == nBar)) {
    SCROLL_DrawMovingThumb(hdc, rect, vertical, arrowSize, thumbSize);
  }
  /* Select the correct brush and pen */
  /* Only scrollbar controls send WM_CTLCOLORSCROLLBAR.
  * The window-owned scrollbars need to call DEFWND_ControlColor
  * to correctly setup default scrollbar colors
  */
  if (nBar == SB_CTL) {
    hBrush = (HBRUSH)SendMessageW(GetParent(hWid), WM_CTLCOLORSCROLLBAR,
        (WPARAM)hdc, (LPARAM)hWid);
  }
  else {
    hBrush = DEFWND_ControlColor(hdc, CTLCOLOR_SCROLLBAR);
  }
  hSavePen = (HPEN)SelectObject(hdc, SYSCOLOR_GetPen(COLOR_WINDOWFRAME));
  hSaveBrush = (HBRUSH)SelectObject(hdc, hBrush);
  /* Calculate the scroll rectangle */
  r = *rect;
  if (vertical) {
    r.top += arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
    r.bottom -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
  }
  else {
    r.left += arrowSize - SCROLL_ARROW_THUMB_OVERLAP;
    r.right -= (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
  }
  /* Draw the scroll bar frame */
  /* Draw the scroll rectangles and thumb */
  if (!thumbPos) { /* No thumb to draw */
    PatBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top, PATCOPY);
    /* cleanup and return */
    SelectObject(hdc, hSavePen);
    SelectObject(hdc, hSaveBrush);
    return;
  }
  if (vertical) {
    PatBlt(hdc, r.left, r.top, r.right - r.left,
        thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP),
        top_selected ? 0x0f0000 : PATCOPY);
    r.top += thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
    PatBlt(hdc, r.left, r.top + thumbSize, r.right - r.left,
        r.bottom - r.top - thumbSize,
        bottom_selected ? 0x0f0000 : PATCOPY);
    r.bottom = r.top + thumbSize;
  }
  else { /* horizontal */
    PatBlt(hdc, r.left, r.top,
        thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP),
        r.bottom - r.top, top_selected ? 0x0f0000 : PATCOPY);
    r.left += thumbPos - (arrowSize - SCROLL_ARROW_THUMB_OVERLAP);
    PatBlt(hdc, r.left + thumbSize, r.top, r.right - r.left - thumbSize,
        r.bottom - r.top, bottom_selected ? 0x0f0000 : PATCOPY);
    r.right = r.left + thumbSize;
  }
  /* Draw the thumb */
  SelectObject(hdc, GetSysColorBrush(COLOR_BTNFACE));
  Rectangle(hdc, r.left + 1, r.top + 1, r.right - 1, r.bottom - 1);
  DrawEdge(hdc, &r, EDGE_RAISED, BF_RECT);
  if (Save_SCROLL_MovingThumb &&
      (SCROLL_TrackingWin == hWid) &&
      (SCROLL_TrackingBar == nBar)) {
    SCROLL_DrawMovingThumb(hdc, rect, vertical, arrowSize, thumbSize);
  }
  /* cleanup */
  SelectObject(hdc, hSavePen);
  SelectObject(hdc, hSaveBrush);
}
/***********************************************************************
 * SCROLL_DrawScrollBar
 *
 * Redraw the whole scrollbar.
 */
void SCROLL_DrawScrollBar(HWID hWid, HDC hdc, INT nBar, BOOL arrows, BOOL interior)
{
  INT arrowSize, thumbSize, thumbPos;
  RECT rect;
  BOOL vertical;
  SCROLLBAR_INFO* infoPtr = SCROLL_GetInternalInfo(hWid, nBar, TRUE);
  BOOL Save_SCROLL_MovingThumb = SCROLL_MovingThumb;
  DWORD style = GetWidgetLong(hWid, GWL_STYLE);
  if (!(hWid = WIN_GetFullHandle(hWid))) {
    return;
  }
  if (!infoPtr ||
      ((nBar == SB_VERT) && !(style & WS_VSCROLL)) ||
      ((nBar == SB_HORZ) && !(style & WS_HSCROLL))) {
    return;
  }
  if (!WIN_IsWidgetDrawable(hWid, FALSE)) {
    return;
  }
  vertical = SCROLL_GetScrollBarRect(hWid, nBar, &rect,
      &arrowSize, &thumbSize, &thumbPos);
  /* do not draw if the scrollbar rectangle is empty */
  if (IsRectEmpty(&rect)) {
    return;
  }
  if (Save_SCROLL_MovingThumb &&
      (SCROLL_TrackingWin == hWid) &&
      (SCROLL_TrackingBar == nBar)) {
    SCROLL_DrawMovingThumb(hdc, &rect, vertical, arrowSize, thumbSize);
  }
  /* Draw the arrows */
  if (arrows && arrowSize) {
    if (vertical == SCROLL_trackVertical && WidgetGetCapture() == hWid)
      SCROLL_DrawArrows(hdc, infoPtr, &rect, arrowSize, vertical,
          (SCROLL_trackHitTest == SCROLL_TOP_ARROW),
          (SCROLL_trackHitTest == SCROLL_BOTTOM_ARROW));
    else {
      SCROLL_DrawArrows(hdc, infoPtr, &rect, arrowSize, vertical, FALSE, FALSE);
    }
  }
  if (interior)
    SCROLL_DrawInterior(hWid, hdc, nBar, &rect, arrowSize, thumbSize,
        thumbPos, infoPtr->flags, vertical, FALSE, FALSE);
  if (Save_SCROLL_MovingThumb &&
      (SCROLL_TrackingWin == hWid) &&
      (SCROLL_TrackingBar == nBar)) {
    SCROLL_DrawMovingThumb(hdc, &rect, vertical, arrowSize, thumbSize);
  }
  /* if scroll bar has focus, reposition the caret */
  if (hWid == WidgetGetFocus() && (nBar == SB_CTL)) {
    if (!vertical) {
      SetCaretPos(thumbPos + 1, rect.top + 1);
    }
    else {
      SetCaretPos(rect.top + 1, thumbPos + 1);
    }
  }
}
/***********************************************************************
 * SCROLL_DrawSizeGrip
 *
 * Draw the size grip.
 */
static void SCROLL_DrawSizeGrip(HWID hWid, HDC hdc)
{
  RECT rc;
  GetWidgetRect(hWid, &rc);
  FillRect(hdc, &rc, GetSysColorBrush(COLOR_SCROLLBAR));
  rc.left = max(rc.left, rc.right - GetSystemMetrics(SM_CXVSCROLL) - 1);
  rc.top = max(rc.top, rc.bottom - GetSystemMetrics(SM_CYHSCROLL) - 1);
  DrawFrameControl(hdc, &rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
}
/***********************************************************************
 * SCROLL_RefreshScrollBar
 *
 * Repaint the scroll bar interior after a SetScrollRange() or
 * SetScrollPos() call.
 */
static void SCROLL_RefreshScrollBar(HWID hWid, INT nBar, BOOL arrows, BOOL interior)
{
  //HDC hdc = GetDCEx(hWid, 0, DCX_CACHE | ((nBar == SB_CTL) ? 0 : DCX_WINDOW));
#if 1
  InvalidateRect(hWid, NULL, TRUE);
  return ;
#else
  HDC hdc = GetDC(hWid);
  if (!hdc) {
    return;
  }
  SCROLL_DrawScrollBar(hWid, hdc, nBar, arrows, interior);
  ReleaseDC(hWid, hdc);
#endif
}
/***********************************************************************
 * SCROLL_HandleKbdEvent
 *
 * Handle a keyboard event (only for SB_CTL scrollbars with focus).
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * wParam [I] Variable input including enable state
 * lParam [I] Variable input including input point
 */
static void SCROLL_HandleKbdEvent(HWID hWid, WPARAM wParam, LPARAM lParam)
{
  //TRACE("hWid=%p wParam=%ld lParam=%ld\n", hWid, wParam, lParam);
  /* hide caret on first KEYDOWN to prevent flicker */
  if ((lParam & PFD_DOUBLEBUFFER_DONTCARE) == 0) {
    HideCaret(hWid);
  }
  switch (wParam) {
  case VK_PRIOR:
    wParam = SB_PAGEUP;
    break;
  case VK_NEXT:
    wParam = SB_PAGEDOWN;
    break;
  case VK_HOME:
    wParam = SB_TOP;
    break;
  case VK_END:
    wParam = SB_BOTTOM;
    break;
  case VK_UP:
    wParam = SB_LINEUP;
    break;
  case VK_DOWN:
    wParam = SB_LINEDOWN;
    break;
  case VK_LEFT:
    wParam = SB_LINEUP;
    break;
  case VK_RIGHT:
    wParam = SB_LINEDOWN;
    break;
  default:
    return;
  }
  SendMessageW(GetParent(hWid),
      ((GetWidgetLong(hWid, GWL_STYLE) & SBS_VERT) ?
          WM_VSCROLL : WM_HSCROLL), wParam, (LPARAM)hWid);
}
/***********************************************************************
 * SCROLL_HandleScrollEvent
 *
 * Handle a mouse or timer event for the scrollbar.
 * 'pt' is the location of the mouse event in client (for SB_CTL) or
 * windows coordinates.
 */
static void SCROLL_HandleScrollEvent(HWID hWid, INT nBar, UINT msg, POINT pt)
{
  /* Previous mouse position for timer events */
  static POINT prevPt;
  /* Thumb position when tracking started. */
  static UINT trackThumbPos;
  /* Position in the scroll-bar of the last button-down event. */
  static INT lastClickPos;
  /* Position in the scroll-bar of the last mouse event. */
  static INT lastMousePos;
  enum SCROLL_HITTEST hittest;
  HWID hwndOwner, hwndCtl;
  BOOL vertical;
  INT arrowSize, thumbSize, thumbPos;
  RECT rect;
  HDC hdc;
  SCROLLBAR_INFO* infoPtr = SCROLL_GetInternalInfo(hWid, nBar, FALSE);
  if (!infoPtr) {
    return;
  }
  if ((SCROLL_trackHitTest == SCROLL_NOWHERE) && (msg != WM_LBUTTONDOWN)) {
    return;
  }
  if (nBar == SB_CTL && (GetWidgetLong(hWid, GWL_STYLE) & (SBS_SIZEGRIP | SBS_SIZEBOX))) {
    switch (msg) {
    case WM_LBUTTONDOWN: /* Initialise mouse tracking */
      HideCaret(hWid); /* hide caret while holding down LBUTTON */
      WidgetSetCapture(hWid);
      prevPt = pt;
      SCROLL_trackHitTest = hittest = SCROLL_THUMB;
      break;
    case WM_MOUSEMOVE:
      GetWidgetRect(GetParent(GetParent(hWid)), &rect);
      prevPt = pt;
      break;
    case WM_LBUTTONUP:
      ReleaseCapture();
      SCROLL_trackHitTest = hittest = SCROLL_NOWHERE;
      if (hWid == WidgetGetFocus()) {
        ShowCaret(hWid);
      }
      break;
    case WM_SYSTIMER:
      pt = prevPt;
      break;
    }
    return;
  }
  hdc = GetDCEx(hWid, 0, DCX_CACHE | ((nBar == SB_CTL) ? 0 : DCX_WINDOW));
  int saveid = SaveDC(hdc);
  vertical = SCROLL_GetScrollBarRect(hWid, nBar, &rect, &arrowSize, &thumbSize, &thumbPos);
  hwndOwner = (nBar == SB_CTL) ? GetParent(hWid) : hWid;
  hwndCtl = (nBar == SB_CTL) ? hWid : 0;
  switch (msg) {
  case WM_LBUTTONDOWN: /* Initialise mouse tracking */
    HideCaret(hWid); /* hide caret while holding down LBUTTON */
    SCROLL_trackVertical = vertical;
    SCROLL_trackHitTest = hittest = SCROLL_HitTest(hWid, nBar, pt, FALSE);
    lastClickPos = vertical ? (pt.y - rect.top) : (pt.x - rect.left);
    lastMousePos = lastClickPos;
    trackThumbPos = thumbPos;
    prevPt = pt;
    if (nBar == SB_CTL && (GetWidgetLong(hWid, GWL_STYLE) & WS_TABSTOP)) {
      WidgetSetFocus(hWid);
    }
    WidgetSetCapture(hWid);
    break;
  case WM_MOUSEMOVE:
    hittest = SCROLL_HitTest(hWid, nBar, pt, TRUE);
    prevPt = pt;
    break;
  case WM_LBUTTONUP:
    hittest = SCROLL_NOWHERE;
    ReleaseCapture();
    /* if scrollbar has focus, show back caret */
    if (hWid == WidgetGetFocus()) {
      ShowCaret(hWid);
    }
    break;
  case WM_SYSTIMER:
    pt = prevPt;
    hittest = SCROLL_HitTest(hWid, nBar, pt, FALSE);
    break;
  default:
    return; /* Should never happen */
  }
  ////TRACE("Event: hWid=%p bar=%d msg=%s pt=%d,%d hit=%d\n", hWid, nBar, SPY_GetMsgName(msg, hWid), pt.x, pt.y, hittest);
  switch (SCROLL_trackHitTest) {
  case SCROLL_NOWHERE: /* No tracking in progress */
    break;
  case SCROLL_TOP_ARROW:
    SCROLL_DrawArrows(hdc, infoPtr, &rect, arrowSize, vertical, (hittest == SCROLL_trackHitTest), FALSE);
    //InvalidateRect(hWid, NULL, TRUE);
    if (hittest == SCROLL_trackHitTest) {
      if ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER)) {
        SendMessageW(hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
            SB_LINEUP, (LPARAM)hwndCtl);
      }
      SetSystemTimer(hWid, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
          SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY, NULL);
    }
    else {
      KillSystemTimer(hWid, SCROLL_TIMER);
    }
    break;
  case SCROLL_TOP_RECT:
    SCROLL_DrawInterior(hWid, hdc, nBar, &rect, arrowSize, thumbSize, thumbPos, infoPtr->flags, vertical, (hittest == SCROLL_trackHitTest), FALSE);
    //InvalidateRect(hWid, NULL, TRUE);
    if (hittest == SCROLL_trackHitTest) {
      if ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER)) {
        SendMessageW(hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
            SB_PAGEUP, (LPARAM)hwndCtl);
      }
      SetSystemTimer(hWid, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
          SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY, NULL);
    }
    else {
      KillSystemTimer(hWid, SCROLL_TIMER);
    }
    break;
  case SCROLL_THUMB:
    if (msg == WM_LBUTTONDOWN) {
      SCROLL_TrackingWin = hWid;
      SCROLL_TrackingBar = nBar;
      SCROLL_TrackingPos = trackThumbPos + lastMousePos - lastClickPos;
      SCROLL_TrackingVal = SCROLL_GetThumbVal(infoPtr, &rect,
          vertical,
          SCROLL_TrackingPos);
      if (!SCROLL_MovingThumb) {
        //InvalidateRect(hWid, NULL, TRUE);
        SCROLL_DrawMovingThumb(hdc, &rect, vertical, arrowSize, thumbSize);
      }
    }
    else if (msg == WM_LBUTTONUP) {
      if (SCROLL_MovingThumb) {
        SCROLL_DrawMovingThumb(hdc, &rect, vertical, arrowSize, thumbSize);
      }
      SCROLL_DrawInterior(hWid, hdc, nBar, &rect, arrowSize, thumbSize, thumbPos, infoPtr->flags, vertical, FALSE, FALSE);
    }
    else { /* WM_MOUSEMOVE */
      INT pos;
      if (!SCROLL_PtInRectEx(&rect, pt, vertical)) {
        pos = lastClickPos;
      }
      else {
        pt = SCROLL_ClipPos(&rect, pt);
        pos = vertical ? (pt.y - rect.top) : (pt.x - rect.left);
      }
      if ((pos != lastMousePos) || (!SCROLL_MovingThumb)) {
        if (SCROLL_MovingThumb) {
          SCROLL_DrawMovingThumb(hdc, &rect, vertical, arrowSize, thumbSize);
        }
        lastMousePos = pos;
        SCROLL_TrackingPos = trackThumbPos + pos - lastClickPos;
        SCROLL_TrackingVal = SCROLL_GetThumbVal(infoPtr, &rect, vertical, SCROLL_TrackingPos);
        SendMessageW(hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK, SCROLL_TrackingVal), (LPARAM)hwndCtl);
        if (!SCROLL_MovingThumb) {
          SCROLL_DrawMovingThumb(hdc, &rect, vertical, arrowSize, thumbSize);
        }
      }
    }
    break;
  case SCROLL_BOTTOM_RECT:
    SCROLL_DrawInterior(hWid, hdc, nBar, &rect, arrowSize, thumbSize, thumbPos, infoPtr->flags, vertical, FALSE, (hittest == SCROLL_trackHitTest));
    if (hittest == SCROLL_trackHitTest) {
      if ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER)) {
        SendMessageW(hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
            SB_PAGEDOWN, (LPARAM)hwndCtl);
      }
      SetSystemTimer(hWid, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ?
          SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY, NULL);
    }
    else {
      KillSystemTimer(hWid, SCROLL_TIMER);
    }
    break;
  case SCROLL_BOTTOM_ARROW:
    SCROLL_DrawArrows(hdc, infoPtr, &rect, arrowSize, vertical, FALSE, (hittest == SCROLL_trackHitTest));
    if (hittest == SCROLL_trackHitTest) {
      if ((msg == WM_LBUTTONDOWN) || (msg == WM_SYSTIMER)) {
        SendMessageW(hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
            SB_LINEDOWN, (LPARAM)hwndCtl);
      }
      SetSystemTimer(hWid, SCROLL_TIMER, (msg == WM_LBUTTONDOWN) ? SCROLL_FIRST_DELAY : SCROLL_REPEAT_DELAY, NULL);
    }
    else {
      KillSystemTimer(hWid, SCROLL_TIMER);
    }
    break;
  }
  if (msg == WM_LBUTTONDOWN) {
    if (hittest == SCROLL_THUMB) {
      UINT val = SCROLL_GetThumbVal(infoPtr, &rect, vertical, trackThumbPos + lastMousePos - lastClickPos);
      SendMessageW(hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK, val), (LPARAM)hwndCtl);
    }
  }
  if (msg == WM_LBUTTONUP) {
    hittest = SCROLL_trackHitTest;
    SCROLL_trackHitTest = SCROLL_NOWHERE; /* Terminate tracking */
    if (hittest == SCROLL_THUMB) {
      UINT val = SCROLL_GetThumbVal(infoPtr, &rect, vertical, trackThumbPos + lastMousePos - lastClickPos);
      SendMessageW(hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, val), (LPARAM)hwndCtl);
    }
    /* SB_ENDSCROLL doesn't report thumb position */
    SendMessageW(hwndOwner, vertical ? WM_VSCROLL : WM_HSCROLL,
        SB_ENDSCROLL, (LPARAM)hwndCtl);
    /* Terminate tracking */
    SCROLL_TrackingWin = 0;
  }
  RestoreDC(hdc, saveid);
  ReleaseDC(hWid, hdc);
}
/***********************************************************************
 * SCROLL_TrackScrollBar
 *
 * Track a mouse button press on a scroll-bar.
 * pt is in screen-coordinates for non-client scroll bars.
 */
void SCROLL_TrackScrollBar(HWID hWid, INT scrollbar, POINT pt)
{
  MSG msg;
  INT xoffset = 0, yoffset = 0;
  if (scrollbar != SB_CTL) {
    RECT rect;
    GetWidgetRect(hWid, &rect);
    pt.x -= rect.left;
    pt.y -= rect.top;
  }
  SCROLL_HandleScrollEvent(hWid, scrollbar, WM_LBUTTONDOWN, pt);
  do {
    if (!GetMessageW(&msg, 0, 0, 0)) {
      break;
    }
    if (CallMsgFilterW(&msg, MSGF_SCROLLBAR)) {
      continue;
    }
    if (msg.message == WM_LBUTTONUP ||
        msg.message == WM_MOUSEMOVE ||
        (msg.message == WM_SYSTIMER && msg.wParam == SCROLL_TIMER)) {
      pt.x = (short)LOWORD(msg.lParam) + xoffset;
      pt.y = (short)HIWORD(msg.lParam) + yoffset;
      SCROLL_HandleScrollEvent(hWid, scrollbar, msg.message, pt);
    }
    else {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
    if (!IsWidget(hWid)) {
      ReleaseCapture();
      break;
    }
  }
  while (msg.message != WM_LBUTTONUP && WidgetGetCapture() == hWid);
}
/***********************************************************************
 * SCROLL_CreateScrollBar
 *
 * Create a scroll bar
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * lpCreate [I] The style and place of the scroll bar
 */
static void SCROLL_CreateScrollBar(HWID hWid, LPCREATESTRUCTW lpCreate)
{
  LPSCROLLBAR_INFO info = SCROLL_GetInternalInfo(hWid, SB_CTL, TRUE);
  if (!info) {
    return;
  }
  //TRACE("hWid=%p lpCreate=%p\n", hWid, lpCreate);
  if (lpCreate->style & WS_DISABLED) {
    info->flags = ESB_DISABLE_BOTH;
    //TRACE("Created WS_DISABLED scrollbar\n");
  }
  if (lpCreate->style & (SBS_SIZEGRIP | SBS_SIZEBOX)) {
    if (lpCreate->style & SBS_SIZEBOXTOPLEFTALIGN)
      MoveWidget(hWid, lpCreate->x, lpCreate->y, GetSystemMetrics(SM_CXVSCROLL) + 1,
          GetSystemMetrics(SM_CYHSCROLL) + 1, FALSE);
    else if (lpCreate->style & SBS_SIZEBOXBOTTOMRIGHTALIGN)
      MoveWidget(hWid, lpCreate->x + lpCreate->cx - GetSystemMetrics(SM_CXVSCROLL) - 1,
          lpCreate->y + lpCreate->cy - GetSystemMetrics(SM_CYHSCROLL) - 1,
          GetSystemMetrics(SM_CXVSCROLL) + 1,
          GetSystemMetrics(SM_CYHSCROLL) + 1, FALSE);
  }
  else if (lpCreate->style & SBS_VERT) {
    if (lpCreate->style & SBS_LEFTALIGN)
      MoveWidget(hWid, lpCreate->x, lpCreate->y,
          GetSystemMetrics(SM_CXVSCROLL) + 1, lpCreate->cy, FALSE);
    else if (lpCreate->style & SBS_RIGHTALIGN)
      MoveWidget(hWid,
          lpCreate->x + lpCreate->cx - GetSystemMetrics(SM_CXVSCROLL) - 1,
          lpCreate->y,
          GetSystemMetrics(SM_CXVSCROLL) + 1, lpCreate->cy, FALSE);
  }
  else { /* SBS_HORZ */
    if (lpCreate->style & SBS_TOPALIGN)
      MoveWidget(hWid, lpCreate->x, lpCreate->y,
          lpCreate->cx, GetSystemMetrics(SM_CYHSCROLL) + 1, FALSE);
    else if (lpCreate->style & SBS_BOTTOMALIGN)
      MoveWidget(hWid,
          lpCreate->x,
          lpCreate->y + lpCreate->cy - GetSystemMetrics(SM_CYHSCROLL) - 1,
          lpCreate->cx, GetSystemMetrics(SM_CYHSCROLL) + 1, FALSE);
  }
}
/*************************************************************************
 * SCROLL_GetScrollInfo
 *
 * Internal helper for the API function
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 * info [IO] fMask specifies which values to retrieve
 *
 * RETURNS
 * FALSE if requested field not filled (f.i. scroll bar does not exist)
 */
static BOOL SCROLL_GetScrollInfo(HWID hWid, INT nBar, LPSCROLLINFO info)
{
  LPSCROLLBAR_INFO infoPtr;
  /* handle invalid data structure */
  if (!SCROLL_ScrollInfoValid(info)
      || !(infoPtr = SCROLL_GetInternalInfo(hWid, nBar, FALSE))) {
    return FALSE;
  }
  /* fill in the desired scroll info structure */
  if (info->fMask & SIF_PAGE) {
    info->nPage = infoPtr->page;
  }
  if (info->fMask & SIF_POS) {
    info->nPos = infoPtr->curVal;
  }
  if ((info->fMask & SIF_TRACKPOS) && (info->cbSize == sizeof(*info))) {
    info->nTrackPos = (SCROLL_TrackingWin == WIN_GetFullHandle(hWid)) ? SCROLL_TrackingVal : infoPtr->curVal;
  }
  if (info->fMask & SIF_RANGE) {
    info->nMin = infoPtr->minVal;
    info->nMax = infoPtr->maxVal;
  }
  //TRACE("cbSize %02x fMask %04x nMin %d nMax %d nPage %u nPos %d nTrackPos %d\n", info->cbSize, info->fMask, info->nMin, info->nMax, info->nPage, info->nPos, info->nTrackPos);
  return (info->fMask & SIF_ALL) != 0;
}
/*************************************************************************
 * SCROLL_GetScrollBarInfo
 *
 * Internal helper for the API function
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * idObject [I] One of OBJID_CLIENT, OBJID_HSCROLL, or OBJID_VSCROLL
 * info [IO] cbSize specifies the size of the structure
 *
 * RETURNS
 * FALSE if failed
 */
static BOOL SCROLL_GetScrollBarInfo(HWID hWid, LONG idObject, LPSCROLLBARINFO info)
{
  LPSCROLLBAR_INFO infoPtr;
  INT nBar;
  INT nDummy;
  DWORD style = GetWidgetLong(hWid, GWL_STYLE);
  BOOL pressed;
  RECT rect;
  switch (idObject) {
  case OBJID_CLIENT:
    nBar = SB_CTL;
    break;
  case OBJID_HSCROLL:
    nBar = SB_HORZ;
    break;
  case OBJID_VSCROLL:
    nBar = SB_VERT;
    break;
  default:
    return FALSE;
  }
  /* handle invalid data structure */
  if (info->cbSize != sizeof(*info)) {
    return FALSE;
  }
  SCROLL_GetScrollBarRect(hWid, nBar, &info->rcScrollBar, &nDummy,
      &info->dxyLineButton, &info->xyThumbTop);
  /* rcScrollBar needs to be in screen coordinates */
  GetWidgetRect(hWid, &rect);
  OffsetRect(&info->rcScrollBar, rect.left, rect.top);
  info->xyThumbBottom = info->xyThumbTop + info->dxyLineButton;
  infoPtr = SCROLL_GetInternalInfo(hWid, nBar, TRUE);
  if (!infoPtr) {
    return FALSE;
  }
  /* Scroll bar state */
  info->rgstate[0] = 0;
  if ((nBar == SB_HORZ && !(style & WS_HSCROLL))
      || (nBar == SB_VERT && !(style & WS_VSCROLL))) {
    info->rgstate[0] |= STATE_SYSTEM_INVISIBLE;
  }
  if (infoPtr->minVal >= infoPtr->maxVal - max(infoPtr->page - 1, 0)) {
    if (!(info->rgstate[0] & STATE_SYSTEM_INVISIBLE)) {
      info->rgstate[0] |= STATE_SYSTEM_UNAVAILABLE;
    }
    else {
      info->rgstate[0] |= STATE_SYSTEM_OFFSCREEN;
    }
  }
  if (nBar == SB_CTL && !IsWidgetEnabled(hWid)) {
    info->rgstate[0] |= STATE_SYSTEM_UNAVAILABLE;
  }
  pressed = ((nBar == SB_VERT) == SCROLL_trackVertical && WidgetGetCapture() == hWid);
  /* Top/left arrow button state. MSDN says top/right, but I don't believe it */
  info->rgstate[1] = 0;
  if (pressed && SCROLL_trackHitTest == SCROLL_TOP_ARROW) {
    info->rgstate[1] |= STATE_SYSTEM_PRESSED;
  }
  if (infoPtr->flags & ESB_DISABLE_LTUP) {
    info->rgstate[1] |= STATE_SYSTEM_UNAVAILABLE;
  }
  /* Page up/left region state. MSDN says up/right, but I don't believe it */
  info->rgstate[2] = 0;
  if (infoPtr->curVal == infoPtr->minVal) {
    info->rgstate[2] |= STATE_SYSTEM_INVISIBLE;
  }
  if (pressed && SCROLL_trackHitTest == SCROLL_TOP_RECT) {
    info->rgstate[2] |= STATE_SYSTEM_PRESSED;
  }
  /* Thumb state */
  info->rgstate[3] = 0;
  if (pressed && SCROLL_trackHitTest == SCROLL_THUMB) {
    info->rgstate[3] |= STATE_SYSTEM_PRESSED;
  }
  /* Page down/right region state. MSDN says down/left, but I don't believe it */
  info->rgstate[4] = 0;
  if (infoPtr->curVal >= infoPtr->maxVal - 1) {
    info->rgstate[4] |= STATE_SYSTEM_INVISIBLE;
  }
  if (pressed && SCROLL_trackHitTest == SCROLL_BOTTOM_RECT) {
    info->rgstate[4] |= STATE_SYSTEM_PRESSED;
  }
  /* Bottom/right arrow button state. MSDN says bottom/left, but I don't believe it */
  info->rgstate[5] = 0;
  if (pressed && SCROLL_trackHitTest == SCROLL_BOTTOM_ARROW) {
    info->rgstate[5] |= STATE_SYSTEM_PRESSED;
  }
  if (infoPtr->flags & ESB_DISABLE_RTDN) {
    info->rgstate[5] |= STATE_SYSTEM_UNAVAILABLE;
  }
  return TRUE;
}
/*************************************************************************
 * SCROLL_GetScrollPos
 *
 * Internal helper for the API function
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 */
static INT SCROLL_GetScrollPos(HWID hWid, INT nBar)
{
  LPSCROLLBAR_INFO infoPtr = SCROLL_GetInternalInfo(hWid, nBar, FALSE);
  return infoPtr ? infoPtr->curVal : 0;
}
/*************************************************************************
 * SCROLL_GetScrollRange
 *
 * Internal helper for the API function
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 * lpMin [O] Where to store minimum value
 * lpMax [O] Where to store maximum value
 *
 * RETURNS
 * Success: TRUE
 * Failure: FALSE
 */
static BOOL SCROLL_GetScrollRange(HWID hWid, INT nBar, LPINT lpMin, LPINT lpMax)
{
  LPSCROLLBAR_INFO infoPtr = SCROLL_GetInternalInfo(hWid, nBar, FALSE);
  if (lpMin) {
    *lpMin = infoPtr ? infoPtr->minVal : 0;
  }
  if (lpMax) {
    *lpMax = infoPtr ? infoPtr->maxVal : 0;
  }
  return TRUE;
}
/*************************************************************************
 * SCROLL_SetScrollRange
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 * lpMin [I] Minimum value
 * lpMax [I] Maximum value
 *
 */
static BOOL SCROLL_SetScrollRange(HWID hWid, INT nBar, INT minVal, INT maxVal)
{
  LPSCROLLBAR_INFO infoPtr = SCROLL_GetInternalInfo(hWid, nBar, FALSE);
  //TRACE("hWid=%p nBar=%d min=%d max=%d\n", hWid, nBar, minVal, maxVal);
  if (infoPtr) {
    infoPtr->minVal = minVal;
    infoPtr->maxVal = maxVal;
  }
  return TRUE;
}
/***********************************************************************
 * ScrollBarWndProc_common
 */
LRESULT ScrollBarWndProc_common(HWID hWid, UINT message, WPARAM wParam, LPARAM lParam)
{
  BOOL unicode = 0;
  if (!IsWidget(hWid)) {
    return 0;
  }
  switch (message) {
  case WM_CREATE:
    SCROLL_CreateScrollBar(hWid, (LPCREATESTRUCTW)lParam);
    break;
  case WM_ENABLE: {
    SCROLLBAR_INFO* infoPtr;
    if ((infoPtr = SCROLL_GetInternalInfo(hWid, SB_CTL, FALSE))) {
      infoPtr->flags = wParam ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH;
      SCROLL_RefreshScrollBar(hWid, SB_CTL, TRUE, TRUE);
    }
    return 0;
  }
  case WM_LBUTTONDBLCLK:
  case WM_LBUTTONDOWN:
    if (GetWidgetLong(hWid, GWL_STYLE) & SBS_SIZEGRIP) {
      SendMessageW(GetParent(hWid), WM_SYSCOMMAND,
          SC_SIZE + ((GetWidgetLong(hWid, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) ?
              WMSZ_BOTTOMLEFT : WMSZ_BOTTOMRIGHT), lParam);
    }
    else {
      POINT pt;
      pt.x = (short)LOWORD(lParam);
      pt.y = (short)HIWORD(lParam);
      SCROLL_TrackScrollBar(hWid, SB_CTL, pt);
    }
    break;
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
  case WM_SYSTIMER: {
    POINT pt;
    pt.x = (short)LOWORD(lParam);
    pt.y = (short)HIWORD(lParam);
    SCROLL_HandleScrollEvent(hWid, SB_CTL, message, pt);
  }
  break;
  case WM_KEYDOWN:
    SCROLL_HandleKbdEvent(hWid, wParam, lParam);
    break;
  case WM_KEYUP:
    ShowCaret(hWid);
    break;
  case WM_SETFOCUS: {
    /* Create a caret when a ScrollBar get focus */
    RECT rect;
    int arrowSize, thumbSize, thumbPos, vertical;
    vertical = SCROLL_GetScrollBarRect(hWid, SB_CTL, &rect,
        &arrowSize, &thumbSize, &thumbPos);
    if (!vertical) {
      CreateCaret(hWid, (HBITMAP)1, thumbSize - 2, rect.bottom - rect.top - 2);
      SetCaretPos(thumbPos + 1, rect.top + 1);
    }
    else {
      CreateCaret(hWid, (HBITMAP)1, rect.right - rect.left - 2, thumbSize - 2);
      SetCaretPos(rect.top + 1, thumbPos + 1);
    }
    ShowCaret(hWid);
  }
  break;
  case WM_KILLFOCUS: {
    RECT rect;
    int arrowSize, thumbSize, thumbPos, vertical;
    vertical = SCROLL_GetScrollBarRect(hWid, SB_CTL, &rect, &arrowSize, &thumbSize, &thumbPos);
    if (!vertical) {
      rect.left = thumbPos + 1;
      rect.right = rect.left + thumbSize;
    }
    else {
      rect.top = thumbPos + 1;
      rect.bottom = rect.top + thumbSize;
    }
    HideCaret(hWid);
    InvalidateRect(hWid, &rect, 0);
    DestroyCaret();
  }
  break;
  case WM_ERASEBKGND:
    return 1;
  case WM_GETDLGCODE:
    return DLGC_WANTARROWS; /* Widgets returns this value */
  case WM_PAINT: {
    HDC hdc = (HDC)wParam;
    if (GetWidgetLong(hWid, GWL_STYLE) & SBS_SIZEGRIP) {
      SCROLL_DrawSizeGrip(hWid, hdc);
    }
    else if (GetWidgetLong(hWid, GWL_STYLE) & SBS_SIZEBOX) {
      RECT rc;
      GetWidgetRect(hWid, &rc);
      FillRect(hdc, &rc, GetSysColorBrush(COLOR_SCROLLBAR));
    }
    else {
      SCROLL_DrawScrollBar(hWid, hdc, SB_CTL, TRUE, TRUE);
    }
  }
  break;
  case WM_SETCURSOR:
    if (GetWidgetLong(hWid, GWL_STYLE) & SBS_SIZEGRIP) {
      ULONG_PTR cursor = (ULONG_PTR)((GetWidgetLong(hWid, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) ? IDC_SIZENESW : IDC_SIZENWSE);
      return (LRESULT)SetCursor(LoadCursorA(0, (LPSTR)cursor));
    }
    return 0;//DefWidgetProcW(hWid, message, wParam, lParam);
  case SBM_SETPOS:
    return SetScrollPos(hWid, SB_CTL, wParam, (BOOL)lParam);
  case SBM_GETPOS:
    return SCROLL_GetScrollPos(hWid, SB_CTL);
  case SBM_SETRANGEREDRAW:
  case SBM_SETRANGE: {
    INT oldPos = SCROLL_GetScrollPos(hWid, SB_CTL);
    SCROLL_SetScrollRange(hWid, SB_CTL, wParam, lParam);
    if (message == SBM_SETRANGEREDRAW) {
      SCROLL_RefreshScrollBar(hWid, SB_CTL, TRUE, TRUE);
    }
    if (oldPos != SCROLL_GetScrollPos(hWid, SB_CTL)) {
      return oldPos;
    }
  }
  return 0;
  case SBM_GETRANGE:
    return SCROLL_GetScrollRange(hWid, SB_CTL, (LPINT)wParam, (LPINT)lParam);
  case SBM_ENABLE_ARROWS:
    return EnableScrollBar(hWid, SB_CTL, wParam);
  case SBM_SETSCROLLINFO:
    return SCROLL_SetScrollInfo(hWid, SB_CTL, (SCROLLINFO*)lParam, wParam);
  case SBM_GETSCROLLINFO:
    return SCROLL_GetScrollInfo(hWid, SB_CTL, (SCROLLINFO*)lParam);
  case SBM_GETSCROLLBARINFO:
    return SCROLL_GetScrollBarInfo(hWid, OBJID_CLIENT, (SCROLLBARINFO*)lParam);
  case 0x00e5:
  case 0x00e7:
  case 0x00e8:
  case 0x00ec:
  case 0x00ed:
  case 0x00ee:
  case 0x00ef:
    ERR("unknown Win32 msg %04x wp=%08lx lp=%08lx\n",
        message, wParam, lParam);
    break;
  default:
    if (message >= WM_USER) {
      WARN("unknown msg %04x wp=%04lx lp=%08lx\n", message, wParam, lParam);
    }
#if 0
    if (unicode) {
      return DefWidgetProcW(hWid, message, wParam, lParam);
    }
    else {
      return DefWidgetProcA(hWid, message, wParam, lParam);
    }
#endif
  }
  return 0;
}
/*************************************************************************
 * SetScrollInfo (USER32.@)
 *
 * SetScrollInfo can be used to set the position, upper bound,
 * lower bound, and page size of a scrollbar control.
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 * info [I] Specifies what to change and new values
 * bRedraw [I] Should scrollbar be redrawn afterwards?
 *
 * RETURNS
 * Scrollbar position
 *
 * NOTE
 * For 100 lines of text to be displayed in a window of 25 lines,
 * one would for instance use info->nMin=0, info->nMax=75
 * (corresponding to the 76 different positions of the window on
 * the text), and info->nPage=25.
 */
INT WINAPI SetScrollInfo(HWID hWid, INT nBar, const SCROLLINFO* info, BOOL bRedraw)
{
  //TRACE("hWid=%p nBar=%d info=%p, bRedraw=%d\n", hWid, nBar, info, bRedraw);
  /* Refer SB_CTL requests to the window */
  if (nBar == SB_CTL) {
    return SendMessageW(hWid, SBM_SETSCROLLINFO, bRedraw, (LPARAM)info);
  }
  else {
    return SCROLL_SetScrollInfo(hWid, nBar, info, bRedraw);
  }
}
static INT SCROLL_SetScrollInfo(HWID hWid, INT nBar, LPCSCROLLINFO info, BOOL bRedraw)
{
  /* Update the scrollbar state and set action flags according to
  * what has to be done graphics wise. */
  SCROLLBAR_INFO* infoPtr;
  UINT new_flags;
  INT action = 0;
  /* handle invalid data structure */
  if (!SCROLL_ScrollInfoValid(info)
      || !(infoPtr = SCROLL_GetInternalInfo(hWid, nBar, TRUE))) {
    return 0;
  }
#ifdef _DEBUG
  if (1) {
    //TRACE("hWid=%p bar=%d", hWid, nBar);
    if (info->fMask & SIF_PAGE) {
      //TRACE(" page=%d", info->nPage);
    }
    if (info->fMask & SIF_POS) {
      //TRACE(" pos=%d", info->nPos);
    }
    if (info->fMask & SIF_RANGE) {
      //TRACE(" min=%d max=%d", info->nMin, info->nMax);
    }
    //TRACE("\n");
  }
#endif // _DEBUG
  /* Set the page size */
  if (info->fMask & SIF_PAGE) {
    if (infoPtr->page != info->nPage) {
      infoPtr->page = info->nPage;
      action |= SA_SSI_REFRESH;
    }
  }
  /* Set the scroll pos */
  if (info->fMask & SIF_POS) {
    if (infoPtr->curVal != info->nPos) {
      infoPtr->curVal = info->nPos;
      action |= SA_SSI_REFRESH;
    }
  }
  /* Set the scroll range */
  if (info->fMask & SIF_RANGE) {
    /* Invalid range -> range is set to (0,0) */
    if ((info->nMin > info->nMax) ||
        ((UINT)(info->nMax - info->nMin) >= 0x80000000)) {
      action |= SA_SSI_REFRESH;
      infoPtr->minVal = 0;
      infoPtr->maxVal = 0;
    }
    else {
      if (infoPtr->minVal != info->nMin ||
          infoPtr->maxVal != info->nMax) {
        action |= SA_SSI_REFRESH;
        infoPtr->minVal = info->nMin;
        infoPtr->maxVal = info->nMax;
      }
    }
  }
  /* Make sure the page size is valid */
  if (infoPtr->page < 0) {
    infoPtr->page = 0;
  }
  else if (infoPtr->page > infoPtr->maxVal - infoPtr->minVal + 1) {
    infoPtr->page = infoPtr->maxVal - infoPtr->minVal + 1;
  }
  /* Make sure the pos is inside the range */
  if (infoPtr->curVal < infoPtr->minVal) {
    infoPtr->curVal = infoPtr->minVal;
  }
  else if (infoPtr->curVal > infoPtr->maxVal - max(infoPtr->page - 1, 0)) {
    infoPtr->curVal = infoPtr->maxVal - max(infoPtr->page - 1, 0);
  }
  //TRACE(" new values: page=%d pos=%d min=%d max=%d\n", infoPtr->page, infoPtr->curVal, infoPtr->minVal, infoPtr->maxVal);
  /* don't change the scrollbar state if SetScrollInfo
  * is just called with SIF_DISABLENOSCROLL
  */
  if (!(info->fMask & SIF_ALL)) {
    goto done;
  }
  /* Check if the scrollbar should be hidden or disabled */
  if (info->fMask & (SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL)) {
    new_flags = infoPtr->flags;
    if (infoPtr->minVal >= infoPtr->maxVal - max(infoPtr->page - 1, 0)) {
      /* Hide or disable scroll-bar */
      if (info->fMask & SIF_DISABLENOSCROLL) {
        new_flags = ESB_DISABLE_BOTH;
        action |= SA_SSI_REFRESH;
      }
      else if ((nBar != SB_CTL) && (action & SA_SSI_REFRESH)) {
        action = SA_SSI_HIDE;
      }
    }
    else /* Show and enable scroll-bar only if no page only changed. */
      if (info->fMask != SIF_PAGE) {
        new_flags = ESB_ENABLE_BOTH;
        if ((nBar != SB_CTL) && ((action & SA_SSI_REFRESH))) {
          action |= SA_SSI_SHOW;
        }
      }
    if (infoPtr->flags != new_flags) { /* check arrow flags */
      infoPtr->flags = new_flags;
      action |= SA_SSI_REPAINT_ARROWS;
    }
  }
done:
  if (action & SA_SSI_HIDE) {
    SCROLL_ShowScrollBar(hWid, nBar, FALSE, FALSE);
  }
  else {
    if (action & SA_SSI_SHOW) {
      if (SCROLL_ShowScrollBar(hWid, nBar, TRUE, TRUE)) {
        return infoPtr->curVal; /* SetWidgetPos() already did the painting */
      }
    }
    if (bRedraw) {
      SCROLL_RefreshScrollBar(hWid, nBar, TRUE, TRUE);
    }
    else if (action & SA_SSI_REPAINT_ARROWS) {
      SCROLL_RefreshScrollBar(hWid, nBar, TRUE, FALSE);
    }
  }
  /* Return current position */
  return infoPtr->curVal;
}
/*************************************************************************
 * GetScrollInfo (USER32.@)
 *
 * GetScrollInfo can be used to retrieve the position, upper bound,
 * lower bound, and page size of a scrollbar control.
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 * info [IO] fMask specifies which values to retrieve
 *
 * RETURNS
 * TRUE if SCROLLINFO is filled
 * ( if nBar is SB_CTL, GetScrollInfo returns TRUE even if nothing
 * is filled)
 */
BOOL GetScrollInfo(HWID hWid, INT nBar, LPSCROLLINFO info)
{
  //TRACE("hWid=%p nBar=%d info=%p\n", hWid, nBar, info);
  /* Refer SB_CTL requests to the window */
  if (nBar == SB_CTL) {
    SendMessageW(hWid, SBM_GETSCROLLINFO, 0, (LPARAM)info);
    return TRUE;
  }
  return SCROLL_GetScrollInfo(hWid, nBar, info);
}
/*************************************************************************
 * GetScrollBarInfo (USER32.@)
 *
 * GetScrollBarInfo can be used to retrieve information about a scrollbar
 * control.
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * idObject [I] One of OBJID_CLIENT, OBJID_HSCROLL, or OBJID_VSCROLL
 * info [IO] cbSize specifies the size of SCROLLBARINFO
 *
 * RETURNS
 * TRUE if success
 */
BOOL WINAPI GetScrollBarInfo(HWID hWid, LONG idObject, LPSCROLLBARINFO info)
{
  //TRACE("hWid=%p idObject=%d info=%p\n", hWid, idObject, info);
  /* Refer OBJID_CLIENT requests to the window */
  if (idObject == OBJID_CLIENT) {
    return SendMessageW(hWid, SBM_GETSCROLLBARINFO, 0, (LPARAM)info);
  }
  else {
    return SCROLL_GetScrollBarInfo(hWid, idObject, info);
  }
}
/*************************************************************************
 * SetScrollPos (USER32.@)
 *
 * Sets the current position of the scroll thumb.
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 * nPos [I] New value
 * bRedraw [I] Should scrollbar be redrawn afterwards?
 *
 * RETURNS
 * Success: Scrollbar position
 * Failure: 0
 *
 * REMARKS
 * Note the ambiguity when 0 is returned. Use GetLastError
 * to make sure there was an error (and to know which one).
 */
INT SetScrollPos(HWID hWid, INT nBar, INT nPos, BOOL bRedraw)
{
  SCROLLINFO info;
  SCROLLBAR_INFO* infoPtr;
  INT oldPos;
  if (!(infoPtr = SCROLL_GetInternalInfo(hWid, nBar, FALSE))) {
    return 0;
  }
  oldPos = infoPtr->curVal;
  info.cbSize = sizeof(info);
  info.nPos = nPos;
  info.fMask = SIF_POS;
  SetScrollInfo(hWid, nBar, &info, bRedraw);
  return oldPos;
}
/*************************************************************************
 * GetScrollPos (USER32.@)
 *
 * Gets the current position of the scroll thumb.
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 *
 * RETURNS
 * Success: Current position
 * Failure: 0
 *
 * REMARKS
 * There is ambiguity when 0 is returned. Use GetLastError
 * to make sure there was an error (and to know which one).
 */
INT GetScrollPos(HWID hWid, INT nBar)
{
  //TRACE("hWid=%p nBar=%d\n", hWid, nBar);
  /* Refer SB_CTL requests to the window */
  if (nBar == SB_CTL) {
    return SendMessageW(hWid, SBM_GETPOS, 0, 0);
  }
  else {
    return SCROLL_GetScrollPos(hWid, nBar);
  }
}
/*************************************************************************
 * SetScrollRange (USER32.@)
 * The SetScrollRange function sets the minimum and maximum scroll box positions
 * If nMinPos and nMaxPos is the same value, the scroll bar will hide
 *
 * Sets the range of the scroll bar.
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 * minVal [I] New minimum value
 * maxVal [I] New Maximum value
 * bRedraw [I] Should scrollbar be redrawn afterwards?
 *
 * RETURNS
 * Success: TRUE
 * Failure: FALSE
 */
BOOL SetScrollRange(HWID hWid, INT nBar, INT minVal, INT maxVal, BOOL bRedraw)
{
  SCROLLINFO info;
  //TRACE("hWid=%p nBar=%d min=%d max=%d, bRedraw=%d\n", hWid, nBar, minVal, maxVal, bRedraw);
  info.cbSize = sizeof(info);
  info.fMask = SIF_RANGE;
  info.nMin = minVal;
  info.nMax = maxVal;
  SetScrollInfo(hWid, nBar, &info, bRedraw);
  return TRUE;
}
/*************************************************************************
 * GetScrollRange (USER32.@)
 *
 * Gets the range of the scroll bar.
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 * lpMin [O] Where to store minimum value
 * lpMax [O] Where to store maximum value
 *
 * RETURNS
 * TRUE if values is filled
 */
BOOL GetScrollRange(HWID hWid, INT nBar, LPINT lpMin, LPINT lpMax)
{
  //TRACE("hWid=%p nBar=%d lpMin=%p lpMax=%p\n", hWid, nBar, lpMin, lpMax);
  /* Refer SB_CTL requests to the window */
  if (nBar == SB_CTL) {
    SendMessageW(hWid, SBM_GETRANGE, (WPARAM)lpMin, (LPARAM)lpMax);
  }
  else {
    SCROLL_GetScrollRange(hWid, nBar, lpMin, lpMax);
  }
  return TRUE;
}
/*************************************************************************
 * SCROLL_ShowScrollBar()
 *
 * Back-end for ShowScrollBar(). Returns FALSE if no action was taken.
 */
static BOOL SCROLL_ShowScrollBar(HWID hWid, INT nBar, BOOL fShowH, BOOL fShowV)
{
  ULONG old_style, set_bits = 0, clear_bits = 0;
  //TRACE("hWid=%p bar=%d horz=%d, vert=%d\n", hWid, nBar, fShowH, fShowV);
  switch (nBar) {
  case SB_CTL:
    ShowWidget(hWid, fShowH ? SW_SHOW : SW_HIDE);
    return TRUE;
  case SB_BOTH:
  case SB_HORZ:
    if (fShowH) {
      set_bits |= WS_HSCROLL;
    }
    else {
      clear_bits |= WS_HSCROLL;
    }
    if (nBar == SB_HORZ) {
      break;
    }
    /* fall through */
  case SB_VERT:
    if (fShowV) {
      set_bits |= WS_VSCROLL;
    }
    else {
      clear_bits |= WS_VSCROLL;
    }
    break;
  default:
    return FALSE; /* Nothing to do! */
  }
  old_style = WIN_SetStyle(hWid, set_bits, clear_bits);
  if ((old_style & clear_bits) != 0 || (old_style & set_bits) != set_bits) {
    /* frame has been changed, let the window redraw itself */
    SetWidgetPos(hWid, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
    return TRUE;
  }
  return FALSE; /* no frame changes */
}
/*************************************************************************
 * ShowScrollBar (USER32.@)
 *
 * Shows or hides the scroll bar.
 *
 * PARAMS
 * hWid [I] Handle of window with scrollbar(s)
 * nBar [I] One of SB_HORZ, SB_VERT, or SB_CTL
 * fShow [I] TRUE = show, FALSE = hide
 *
 * RETURNS
 * Success: TRUE
 * Failure: FALSE
 */
BOOL ShowScrollBar(HWID hWid, INT nBar, BOOL fShow)
{
  if (!hWid) {
    return FALSE;
  }
  SCROLL_ShowScrollBar(hWid, nBar, (nBar == SB_VERT) ? 0 : fShow, (nBar == SB_HORZ) ? 0 : fShow);
  return TRUE;
}
/*************************************************************************
 * EnableScrollBar (USER32.@)
 *
 * Enables or disables the scroll bars.
 */
BOOL EnableScrollBar(HWID hWid, UINT nBar, UINT flags)
{
  BOOL bFineWithMe;
  SCROLLBAR_INFO* infoPtr;
  flags &= ESB_DISABLE_BOTH;
  if (nBar == SB_BOTH) {
    if (!(infoPtr = SCROLL_GetInternalInfo(hWid, SB_VERT, TRUE))) {
      return FALSE;
    }
    if (!(bFineWithMe = (infoPtr->flags == flags))) {
      infoPtr->flags = flags;
      SCROLL_RefreshScrollBar(hWid, SB_VERT, TRUE, TRUE);
    }
    nBar = SB_HORZ;
  }
  else {
    bFineWithMe = TRUE;
  }
  if (!(infoPtr = SCROLL_GetInternalInfo(hWid, nBar, TRUE))) {
    return FALSE;
  }
  if (bFineWithMe && infoPtr->flags == flags) {
    return FALSE;
  }
  infoPtr->flags = flags;
  if (nBar == SB_CTL && (flags == ESB_DISABLE_BOTH || flags == ESB_ENABLE_BOTH)) {
    EnableWidget(hWid, flags == ESB_ENABLE_BOTH);
  }
  SCROLL_RefreshScrollBar(hWid, nBar, TRUE, TRUE);
  return TRUE;
}
#define DefWidgetProcW DefWidgetProc
#define DefWidgetProcA DefWidgetProc
LRESULT DefWidgetProc(HWID hWid, UINT message, WPARAM wParam, LPARAM lParam)
{
  BOOL unicode = 0;
  static BOOL lbuttondown = 0;
  if (!IsWidget(hWid)) {
    return 0;
  }
  switch (message) {
  case WM_CREATE:
    //SCROLL_CreateScrollBar(hWid, (LPCREATESTRUCTW)lParam);
    break;
  case WM_SIZE:
    if (hWid->handle) {
      hWid->rect.right = LOWORD(lParam);
      hWid->rect.bottom = HIWORD(lParam);
      InvalidateRect(hWid, NULL, TRUE);
    }
    break;
  case WM_LBUTTONDBLCLK:
  case WM_LBUTTONDOWN: {
    POINT pt;
    pt.x = (short)LOWORD(lParam);
    pt.y = (short)HIWORD(lParam);
    lbuttondown = 1;
    if (hWid->dwStyle & WS_HSCROLL && ((!SCROLL_trackVertical && SCROLL_NOWHERE != SCROLL_trackHitTest) || SCROLL_NOWHERE != SCROLL_HitTest(hWid, SB_HORZ, pt, FALSE))) {
      //SCROLL_HandleScrollEvent(hWid, SB_HORZ, message, pt);
    }
    if (hWid->dwStyle & WS_VSCROLL && ((SCROLL_trackVertical && SCROLL_NOWHERE != SCROLL_trackHitTest) || SCROLL_NOWHERE != SCROLL_HitTest(hWid, SB_VERT, pt, FALSE))) {
      SCROLL_HandleScrollEvent(hWid, SB_VERT, message, pt);
    }
  }
  break;
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
  case WM_SYSTIMER:
    if (lbuttondown) {
      POINT pt;
      pt.x = (short)LOWORD(lParam);
      pt.y = (short)HIWORD(lParam);
      if (lbuttondown && WM_MOUSEMOVE == message) {
      }
      if (hWid->dwStyle & WS_HSCROLL) {
        SCROLL_HandleScrollEvent(hWid, SB_HORZ, message, pt);
      }
      if (hWid->dwStyle & WS_VSCROLL) {
        SCROLL_HandleScrollEvent(hWid, SB_VERT, message, pt);
      }
      if (WM_LBUTTONUP == message) {
        lbuttondown = 0;
      }
    }
    break;
  case WM_KEYDOWN:
    SCROLL_HandleKbdEvent(hWid, wParam, lParam);
    break;
  case WM_KEYUP:
    ShowCaret(hWid);
    break;
  case WM_GETDLGCODE:
    return DLGC_WANTARROWS; /* Widgets returns this value */
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWid, &ps);
    if (hWid->dwStyle & WS_HSCROLL) {
      SCROLL_DrawScrollBar(hWid, hdc, SB_HORZ, TRUE, TRUE);
    }
    if (hWid->dwStyle & WS_VSCROLL) {
      SCROLL_DrawScrollBar(hWid, hdc, SB_VERT, TRUE, TRUE);
    }
    EndPaint(hWid, &ps);
  }
  break;
  default:
    if (message >= WM_USER) {
      //WARN("unknown msg %04x wp=%04lx lp=%08lx\n", message, wParam, lParam);
    }
  }
  return 0;
}

