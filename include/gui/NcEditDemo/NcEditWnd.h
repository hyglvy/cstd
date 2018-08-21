//! NC �༭���࣬����װ��Scintilla
/*! 
 \author   : ������� 
 \date     : 2008/12/13
 \version  : ver 0.1
 \filename : NcEditWnd.h
 \note     : 
        
 \attention: SciLexer.dll�ļ��������Ӧ�ó������ڵ�Ŀ¼��(%PATH%�п����ҵ�Ҳ����)\n

        
 \remark   : 1.2008/12/13 14:02 created by ������� version 0.1  
 \History:
   <author>       <time>       <version>         <desc>	
*/
#pragma once
#include <afxwin.h>
#include "Scintilla.h"
#include "SciLexer.h"

class CNcEditWnd : public CWnd
{
public:
	/**
	* ����ʱ����Scintilla.dll  
	* @param void
	* @return 
	* @see
	*/
	CNcEditWnd(void);
	/**
	* �ͷ�Scintilla.dll    
	* @param void
	* @return 
	* @see
	*/
	~CNcEditWnd(void);
protected:
	DECLARE_MESSAGE_MAP()
public:
	
	/**
	* ����NC�༭�����ڡ�
	* @param DWORD dwExStyle��������ʽ
	* @param const RECT & rect��λ�����С
	* @param CWnd * pParentWnd��������
	* @return BOOL���ɹ����
	* @see
	*/
	virtual BOOL Create(DWORD dwExStyle,const RECT& rect, CWnd* pParentWnd);
	
	/**
	* ��ʼ���༭�����ڵĸ������ԡ�  
	* @param int nSize���ֺ�
	* @param const TCHAR * face����������
	* @return void
	* @see
	*/
	void InitNcEdit(int nSize,const TCHAR* face);

	/**
	* ��NC����  
	* @param LPCTSTR lpFileName
	* @return BOOL
	* @see
	*/
	BOOL OpenFile(LPCTSTR lpFileName);
	
	/**
	* ���ص�ǰ������ڵ��е��к�  
	* @param void
	* @return UINT
	* @see
	*/
	UINT GetCurrentLine(void);
	
	/**
	* ˢ�´�����ʽ���ڸ����ڵ�OnNotify�и�����Ҫ��������
	* @param void
	* @return void
	* @see
	*/
	void UpdateStyle(void);

	/**
	* �Զ�����  
	* @param BOOL bWraped
	* @return void
	* @see
	*/
	void SetWrap(BOOL bWraped);

	/**
	* ��ʾ��β��־  
	* @param BOOL bShow
	* @return void
	* @see
	*/
	void ShowLineEndings(BOOL bShow);

	/**
	* ��ָ����������Ϊѡ��״̬  
	* @param int nLine
	* @return void
	* @see
	*/
	void SelectLine(int nLine);

	/**
	* �����ļ���\n
	* ����ָ��strFileName����strFileNameΪ�գ���ԭ�ļ����洢�� 
	* @param CString & strFileName
	* @return BOOL
	* @see
	*/
	BOOL SaveFile(CString strFileName=_T(""));
private:
	HMODULE m_hSciLexerDll;
	CString m_strFileName;
private:
	void SetDefaultColorFont(int nSize,const TCHAR* face);
	void UpdateLineNumberWidth(void);
	void SetNCEditStyle(void);
};
