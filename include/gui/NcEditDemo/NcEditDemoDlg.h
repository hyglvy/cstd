// NcEditDemoDlg.h : ͷ�ļ�
//

#pragma once

#include "NcEditWnd.h"
// CNcEditDemoDlg �Ի���
class CNcEditDemoDlg : public CDialog
{
// ����
public:
	CNcEditDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CNcEditDemoDlg();
// �Ի�������
	enum { IDD = IDD_NCEDITDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	CNcEditWnd *m_pNcEditWnd;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMenuOpen();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
