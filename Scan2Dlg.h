// Scan2Dlg.h : header file
//

#if !defined(AFX_SCAN2DLG_H__717E1E10_9CC3_4634_92E3_97796982929D__INCLUDED_)
#define AFX_SCAN2DLG_H__717E1E10_9CC3_4634_92E3_97796982929D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CScan2Dlg dialog

class CScan2Dlg : public CDialog
{
// Construction
public:
	void Play(CString str);
	
	CScan2Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CScan2Dlg)
	enum { IDD = IDD_SCAN2_DIALOG };
	CListCtrl	m_list_ip;
	CEdit	m_ret;
	CButton	m_ctrBeginScan;
	CListCtrl	m_port;
	CIPAddressCtrl	m_ip2;
	CIPAddressCtrl	m_ip1;
	int		m_cnt;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScan2Dlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CScan2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void Onsetport();
	afx_msg void Onport();
	afx_msg void OnDestroy();
	afx_msg void OnIp();
	//}}AFX_MSG
	afx_msg void OnScanThread(WPARAM wParam, LPARAM lParam);
	afx_msg void OnScanThreadIp(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	short m_nHasComplete;
	HANDLE m_hThread;
	DWORD m_dwPercentSeconds;
	HTREEITEM m_hParentTree;
	BOOL m_bTerminate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCAN2DLG_H__717E1E10_9CC3_4634_92E3_97796982929D__INCLUDED_)
