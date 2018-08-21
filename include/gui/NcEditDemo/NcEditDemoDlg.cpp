// NcEditDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NcEditDemo.h"
#include "NcEditDemoDlg.h"
#include ".\nceditdemodlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CNcEditDemoDlg �Ի���



CNcEditDemoDlg::CNcEditDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNcEditDemoDlg::IDD, pParent)
	,m_pNcEditWnd(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CNcEditDemoDlg::~CNcEditDemoDlg()
{
	if (m_pNcEditWnd!=NULL){
		delete m_pNcEditWnd;
	}
}

void CNcEditDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNcEditDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_MENU_OPEN, OnMenuOpen)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CNcEditDemoDlg ��Ϣ�������

BOOL CNcEditDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CNcEditDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CNcEditDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CNcEditDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CNcEditDemoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rcWin;
	GetClientRect(&rcWin);
	m_pNcEditWnd=new CNcEditWnd();
	m_pNcEditWnd->Create(WS_EX_CLIENTEDGE,rcWin,this);
	m_pNcEditWnd->InitNcEdit(12,"Fixedsys");
	m_pNcEditWnd->ShowWindow(SW_SHOW);

	return 0;
}

void CNcEditDemoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CRect rcWin;
	GetClientRect(&rcWin);
	m_pNcEditWnd->MoveWindow(&rcWin);
	// TODO: �ڴ˴������Ϣ����������
}

void CNcEditDemoDlg::OnMenuOpen()
{
	// TODO: �ڴ���������������
	CFileDialog fDlg(TRUE);
	if (IDOK==fDlg.DoModal())
	{
		m_pNcEditWnd->OpenFile(fDlg.GetPathName());
	}
}

BOOL CNcEditDemoDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: �ڴ����ר�ô����/����û���
	struct SCNotification* scn = (struct SCNotification*)lParam;
	LPNMHDR pnmh = (LPNMHDR)lParam;
	switch(pnmh->code)
	{
	case SCN_MODIFIED://�޸����ļ�
	case SCN_ZOOM://�Ŵ���С
		m_pNcEditWnd->UpdateStyle();
		break;
	case SCN_UPDATEUI://�������(������꣬���¼�ͷ��)
		int n=m_pNcEditWnd->GetCurrentLine();
		//��ǰ��
		break;
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CNcEditDemoDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	TCHAR szBufferName[MAX_PATH];
	DragQueryFile(hDropInfo,0,szBufferName,MAX_PATH);
	m_pNcEditWnd->OpenFile(szBufferName);
	CDialog::OnDropFiles(hDropInfo);
}
