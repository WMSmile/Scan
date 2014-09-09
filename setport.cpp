// setport.cpp : implementation file
//

#include "stdafx.h"
#include "Scan2.h"
#include "setport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// setport dialog


setport::setport(CWnd* pParent /*=NULL*/)
	: CDialog(setport::IDD, pParent)
{
	//{{AFX_DATA_INIT(setport)
	m_port1 = 0;
	m_port2 = 0;
	//}}AFX_DATA_INIT
}


void setport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(setport)
	DDX_Text(pDX, IDC_port1, m_port1);
	DDX_Text(pDX, IDC_port2, m_port2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(setport, CDialog)
	//{{AFX_MSG_MAP(setport)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// setport message handlers
