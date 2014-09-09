// Scan2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Scan2.h"
#include "Scan2Dlg.h"
#include "setport.h"
#include "new.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
volatile BOOL m_bRunFlag=TRUE;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
int minport=0;
int maxport=5000;
//�̷߳��صĴ�����룬������
#define ERROR_CREATE 		-1L
#define ERROR_MODIFY_FIONBIO	-2L
#define ERROR_SELECT			-3L
#define ERROR_SELECT_TIMEOUT	-4L

//�̷߳��ص���Ϣ
#define SCAN_THREAD				WM_USER+150
#define SCAN_THREADIP				WM_USER+100
//�̷߳��ص���Ϣ��wParam��������
#define DOSCAN_FIND		1		//����һ�����Ŷ˿�
#define DOSCAN_END			2		//���һ���˿�ɨ��
#define STARTSCAN_COMPLETE		3		//�������ɨ��

//ȫ�ֱ���
int cnt;
PICMP_ECHO_REPLY pIpe;
char acPingBuffer[64];
HANDLE hIcmp;
HINSTANCE hInst;
HWND g_hWnd = NULL;						//������Ϣ�Ĵ��ھ��
unsigned long g_ulAddr = INADDR_NONE;	//ɨ���������ַ
DWORD g_dwTimeOut = 1000;				//���ӳ�ʱʱ�䣬��ms��
bool g_bTerminate = false;				//�Ƿ��û���������ɨ��ı�־
short g_nMaxThread = 200;				//��������ɨ���߳����������鲻�˴���200
short g_nThreadCount = 0;				//��ǰ����ɨ��Ľ�����

lpIcmpCreateFile IcmpCreateFile;
lpIcmpSendEcho IcmpSendEcho;
lpIcmpCloseHandle IcmpCloseHandle;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

struct tag_PORTS //�˿ڽṹ��
{
	int iStartPort;
	int iEndPort;			
};
struct tag_IP  //IP��ַ�ṹ��
{
	unsigned long s_ip;
	unsigned long e_ip;
};
DWORD InvertIp(DWORD IP) //��ַת��
{
	unsigned   char   b1,b2,b3,b4;  

	b1   =   IP&0x00FF;
	b2   =   (IP>>8) & 0x00FF; 
	b3   =   (IP>>16) & 0x00FF;  
	b4   =   (IP>>24) & 0x00FF; 

	return   (b1<<24)|(b2<<16)|(b3<<8)|b4;
}
DWORD WINAPI DoScanIp(LPVOID lpParam) //�ж�ip�Ƿ�����
{
	DWORD dwRet;
	DWORD IP = *(DWORD*)	lpParam; //�õ�IP��ַ
	delete lpParam;

	CString str_ip,str;
	in_addr   ip;
	ip.S_un.S_addr=InvertIp(IP);
	str_ip=inet_ntoa(ip); //��ʽת��

	DWORD dwStatus = IcmpSendEcho(hIcmp,ip.S_un.S_addr,acPingBuffer, //����icmp���Ľ����ж�
	sizeof(acPingBuffer),NULL,pIpe,sizeof(ICMP_ECHO_REPLY) + sizeof(acPingBuffer),TIMEOUT);
	if(dwStatus!=0 && inet_ntoa(*(in_addr*)(&pIpe->Address))==str_ip) //�ɹ�
	{
		dwRet = ERROR_SUCCESS;
	}

	g_nThreadCount --; //�߳�����һ
	if (dwRet == ERROR_SUCCESS)
	{
		::SendMessage(g_hWnd, SCAN_THREADIP, DOSCAN_FIND, IP); //���ͳɹ���Ϣ��OnScanThreadIp
	}
	else
	{
		::SendMessage(g_hWnd, SCAN_THREADIP, DOSCAN_END, IP);
	}
	return dwRet;
}
DWORD WINAPI DoScanPort(LPVOID lpParam)  //�ж϶˿��Ƿ��
{
	DWORD dwRet;
	int nPort = *(int*)	lpParam; //��ö˿ں�
	delete lpParam;
	
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); //�����׽���
	if(sock == INVALID_SOCKET)
	{
		AfxMessageBox("�����׽���ʧ��!");
		dwRet = ERROR_CREATE;
	}
	else
	{		
		unsigned long flag = 1; 	
		if ((ioctlsocket(sock, FIONBIO, &flag) != 0))
		{
			AfxMessageBox("δ�ܸ�Ϊ������ģʽ!");
			dwRet = ERROR_MODIFY_FIONBIO;
		}
		else
		{
			//�׽�������
			sockaddr_in severAddr;
			severAddr.sin_family = AF_INET;
			severAddr.sin_port = htons(nPort); 		
			severAddr.sin_addr.S_un.S_addr = g_ulAddr;
			connect(sock, (sockaddr*)&severAddr, sizeof(severAddr));
			
			struct fd_set mask;			
			FD_ZERO(&mask);
			FD_SET(sock, &mask);
			
			struct timeval timeout;
			timeout.tv_sec = g_dwTimeOut / 1000;
			timeout.tv_usec = g_dwTimeOut % 1000;
			//select��ʹ��
			switch(select(0, NULL, &mask, NULL, &timeout))
			{
			case -1:
				dwRet = ERROR_SELECT;
				break;
				
			case 0:
				dwRet = ERROR_SELECT_TIMEOUT;
				break;
				
			default:		
				dwRet = ERROR_SUCCESS;
			};
		}		
		closesocket(sock);
	}
	g_nThreadCount --; //�߳�����һ
	if (dwRet == ERROR_SUCCESS) //����Ϣת����OnScanThread
	{
		::SendMessage(g_hWnd, SCAN_THREAD, DOSCAN_FIND, nPort);
	}
	else
	{
		::SendMessage(g_hWnd, SCAN_THREAD, DOSCAN_END, nPort);
	}
	return dwRet;
}

DWORD WINAPI StartScanIp(LPVOID lpParam) //IPɨ���̺߳���
{
	tag_IP* IPParam = (tag_IP*)lpParam; //ip�ṹ��
	
	DWORD dwThreadId;
	DWORD i;
	for(i=IPParam->s_ip; i<=IPParam->e_ip; i++) 
	{
		while(g_nThreadCount >= g_nMaxThread) //�߳������ڸ��������ǵȴ�
		{
			Sleep(10);
		}	
		
		if (CreateThread(NULL, 0, DoScanIp, (LPVOID)new DWORD(i), 0, &dwThreadId) != NULL) //�����߳�
		{
			g_nThreadCount ++;
		}
	}
	
	//�ȴ����˿�ɨ���߳̽���
	while (g_nThreadCount > 0)
	{
		Sleep(50);
	}
	::SendMessage(g_hWnd, SCAN_THREADIP, STARTSCAN_COMPLETE, 0);
	delete IPParam;
	return ERROR_SUCCESS;
}


DWORD WINAPI StartScan(LPVOID lpParam) //�˿�ɨ���߳�
{	
	tag_PORTS* pScanParam = (tag_PORTS*)lpParam; //�˿ڽṹ��
	
	DWORD dwThreadId;
	int i;
	for(i=pScanParam->iStartPort; i<=pScanParam->iEndPort; i++)
	{
		if (g_bTerminate)	
		{
			break;	//�û��ѷ�������ɨ������
		}
		while(g_nThreadCount >= g_nMaxThread) //�߳������ڸ��������ǵȴ�
		{
			Sleep(10);
		}	
		
		if (CreateThread(NULL, 0, DoScanPort, (LPVOID)new int(i), 0, &dwThreadId) != NULL)//�����߳�
		{
			g_nThreadCount ++;
		}
	}
	
	//�ȴ����˿�ɨ���߳̽���
	while (g_nThreadCount > 0)
	{
		Sleep(50);
	}
	::SendMessage(g_hWnd, SCAN_THREAD, STARTSCAN_COMPLETE, 0);
	delete pScanParam;
	return ERROR_SUCCESS;
}


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScan2Dlg dialog

CScan2Dlg::CScan2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScan2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScan2Dlg)
	m_cnt = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hThread=NULL;
}

void CScan2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScan2Dlg)
	DDX_Control(pDX, IDC_LIST_IP, m_list_ip);
	DDX_Control(pDX, IDC_retport, m_ret);
	DDX_Control(pDX, IDC_port, m_ctrBeginScan);
	DDX_Control(pDX, IDC_IP2, m_ip2);
	DDX_Control(pDX, IDC_IP1, m_ip1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScan2Dlg, CDialog)
	//{{AFX_MSG_MAP(CScan2Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_setport, Onsetport)
	ON_BN_CLICKED(IDC_port, Onport)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_IP, OnIp)
	//}}AFX_MSG_MAP
ON_MESSAGE(SCAN_THREADIP, OnScanThreadIp)  //IP�жϽ����Ϣת��
ON_MESSAGE(SCAN_THREAD, OnScanThread)   //�˿ڽ����Ϣת��
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScan2Dlg message handlers

BOOL CScan2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	CFont *m_Font;
    m_Font = new CFont; 
    m_Font->CreateFont(25,10,0,0,50,
    FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_SWISS,"Arial");
    CEdit *m_Edit=(CEdit *)GetDlgItem(IDC_IP1);
    m_Edit->SetFont(m_Font,FALSE);
    GetDlgItem(IDC_IP1)->SetFont(m_Font);

	m_Edit=(CEdit *)GetDlgItem(IDC_IP2);
    m_Edit->SetFont(m_Font,FALSE);
    GetDlgItem(IDC_IP2)->SetFont(m_Font);

	m_Edit=(CEdit *)GetDlgItem(IDC_retport);
    m_Edit->SetFont(m_Font,FALSE);
    GetDlgItem(IDC_retport)->SetFont(m_Font);

	m_list_ip.SetFont(m_Font);

	m_list_ip.InsertColumn(0,"IP");
	m_list_ip.SetColumnWidth(0,255);

	
	UpdateData(false);
	WSADATA wsaData;	
	BOOL ret = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(ret != 0)
	{
		MessageBox("��ʼ������Э��ʧ��!");
		return -1;
	}
	hInst = LoadLibrary("ICMP.dll");
	if (!hInst)
	{
		AfxMessageBox("Could not load up the ICMP DLL!");
		WSACleanup();
		return 0;
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScan2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScan2Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScan2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CScan2Dlg::Onsetport() 
{
	// TODO: Add your control notification handler code here
	setport sport;
	if(sport.DoModal()==IDOK)
	{
		//sport.UpdateData();
		minport=sport.m_port1;
		maxport=sport.m_port2;
	}
	else
	{
		minport=0;
		maxport=5000;
	}
	m_hThread=NULL;
}

BOOL CScan2Dlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		return 0;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CScan2Dlg::OnScanThread(WPARAM wParam, LPARAM lParam)
{
	CString str;
	switch(wParam)
	{
	case DOSCAN_FIND:
		str.Format("\t%d\n",(int)(lParam));
		Play(str);
		
	case DOSCAN_END:
		m_nHasComplete++;
		break;
		
	case STARTSCAN_COMPLETE:
		m_hThread = NULL;
		m_ctrBeginScan.SetWindowText("��ʼɨ��");
		KillTimer(1);
		AfxMessageBox("����ɨ�����");
		break;
		
	default:
		break;		
	}
}
void CScan2Dlg::OnScanThreadIp(WPARAM wParam, LPARAM lParam)
{
	CString str;
	in_addr   ip;
	switch(wParam)
	{
	case DOSCAN_FIND:
		ip.S_un.S_addr=InvertIp((DWORD)lParam);
		str=inet_ntoa(ip);
		m_list_ip.InsertItem(cnt,str);
		m_list_ip.SetRedraw(TRUE);
		m_list_ip.Invalidate();
		m_list_ip.UpdateWindow();
		
	case DOSCAN_END:
		m_nHasComplete++;
		break;
		
	case STARTSCAN_COMPLETE:
		m_hThread = NULL;
		KillTimer(1);
		AfxMessageBox("����ɨ�����");
		break;	
	default:
		break;		
	}
}
void CScan2Dlg::Onport() 
{
	// TODO: Add your control notification handler code here
	m_ret.SetSel(0,-1);
	m_ret.Clear();
	int cnt=m_list_ip.GetSelectionMark();
	if(cnt<0)
	{
		MessageBox("����ѡ��IP��ַ������");
		return ;
	}
	CString ip=m_list_ip.GetItemText(cnt,0),str;//*/
	str.Format("����ɨ�� %s �Ķ˿����......\n",ip);
	Play(str);
	if (m_hThread == NULL)
	{
		UpdateData();
		g_ulAddr=inet_addr(ip);
		g_bTerminate = false;
		g_hWnd = m_hWnd;
		g_nThreadCount = 0;
		
		tag_PORTS* pScanParam = new tag_PORTS;
		pScanParam->iStartPort = minport;
		pScanParam->iEndPort = maxport;
		
		DWORD dwThreadId;
		m_hThread = CreateThread(NULL, 0, StartScan, (LPVOID)pScanParam, 0, &dwThreadId);
		m_ctrBeginScan.SetWindowText("����ɨ��");
		SetTimer(1, 100, NULL);
	}
	else if (!g_bTerminate)
	{
		g_bTerminate = TRUE;
		m_ctrBeginScan.SetWindowText("���ڽ���...");
		Sleep(100);
		m_ctrBeginScan.SetWindowText("��ʼɨ��");
	}
}

void CScan2Dlg::Play(CString str)
{
	LPCTSTR pDisplay;
	pDisplay = _T(str);//ת����LPCTSTR��������
	int nlength = m_ret.GetWindowTextLength();//��ȡ�༭����ַ���
	m_ret.SetSel(nlength, nlength);//��λ���
	m_ret.ReplaceSel(pDisplay);//��괦��ʾ
}

void CScan2Dlg::OnDestroy() 
{
	TerminateThread(m_hThread, 0);
	m_hThread = NULL;	
	KillTimer(1);
	WSACleanup();
	CDialog::OnDestroy();
}
void CScan2Dlg::OnIp() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	BYTE f4,f1,f2,f3;//����
	CString str;
	DWORD mi,ma;
	//�������û��������ַ
	if (m_ip1.IsBlank() || m_ip2.IsBlank())
	{
		MessageBox(_T("�����IP��ַ������"),
			_T("Error"),
			MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	//�������IP
	if (m_ip1.GetAddress(f1,f2,f3,f4) < 4)
	{
		MessageBox(_T("IP������󣡣���"),
			_T("Invalid IP address"),MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	mi=(DWORD)(f1<<24)|(f2<<16)|(f3<<8)|f4;

	if (m_ip2.GetAddress(f1,f2,f3,f4) < 4)
	{
		MessageBox(_T("IP������󣡣���"),
			_T("Invalid IP address"),MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	ma=(f1<<24)|(f2<<16)|(f3<<8)|f4;
	IcmpCreateFile = (lpIcmpCreateFile)GetProcAddress(hInst, "IcmpCreateFile");
	IcmpSendEcho = (lpIcmpSendEcho)GetProcAddress(hInst, "IcmpSendEcho");
	IcmpCloseHandle = (lpIcmpCloseHandle)GetProcAddress(hInst, "IcmpCloseHandle");
	if (IcmpCreateFile==NULL || IcmpSendEcho==NULL || IcmpCloseHandle==NULL)
	{
		//cout << "Could not find ICMP functions in the ICMP DLL\n";
		str="\nCould not find ICMP functions in the ICMP DLL!";
		AfxMessageBox(str);
		return ;
	}
	//��ICMP���
	hIcmp = IcmpCreateFile();
	if (hIcmp == INVALID_HANDLE_VALUE)
	{
		//cout << "Could not get a valid ICMP handle\n";
		str="\nCould not get a valid ICMP handle!";
		AfxMessageBox(str);
		return ;
	}//*/

	// ����ping���ݰ�
	memset(acPingBuffer, '\xAA', sizeof(acPingBuffer));
	//�����ڴ�ռ�
	pIpe = (PICMP_ECHO_REPLY)GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT,
	sizeof(ICMP_ECHO_REPLY) + sizeof(acPingBuffer)); 
	if (pIpe == 0) 
	{
		//cerr << "Failed to allocate global ping packet buffer." << endl;
		str="Failed to allocate global ping packet buffer.";
		AfxMessageBox(str);
		return ;
	}
	pIpe->Data = acPingBuffer;
	pIpe->DataSize = sizeof(acPingBuffer);

	cnt=0;
	
	m_list_ip.DeleteAllItems();
	m_list_ip.SetRedraw(0);


	UpdateData();
	g_hWnd = m_hWnd;
	g_nThreadCount = 0;

	tag_IP* IPParam = new tag_IP;
	IPParam->s_ip = mi;
	IPParam->e_ip = ma;
	
	DWORD dwThreadId;
	m_hThread = CreateThread(NULL,0,StartScanIp, (LPVOID)IPParam, 0, &dwThreadId);

	//SetTimer(1, 100, NULL);
}

