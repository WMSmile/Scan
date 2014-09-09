#if !defined(AFX_SETPORT_H__422FF761_4E0D_4CEC_BFCE_5925965F9FEA__INCLUDED_)
#define AFX_SETPORT_H__422FF761_4E0D_4CEC_BFCE_5925965F9FEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// setport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// setport dialog

class setport : public CDialog
{
// Construction
public:
	setport(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(setport)
	enum { IDD = IDD_setport };
	int		m_port1;
	int		m_port2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(setport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(setport)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETPORT_H__422FF761_4E0D_4CEC_BFCE_5925965F9FEA__INCLUDED_)
