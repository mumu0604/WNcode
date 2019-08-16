// DynEdit.cpp : implementation file
//

#include "stdafx.h"
#include "QKDTrans.h"
#include "DynEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynEdit

CDynEdit::CDynEdit(int id, int bitLength, double coef)
{
	m_id = id;
	m_coef = fabs(coef);
	m_bitLength = bitLength;
}

CDynEdit::~CDynEdit()
{
}


BEGIN_MESSAGE_MAP(CDynEdit, CEdit)
	//{{AFX_MSG_MAP(CDynEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynEdit message handlers

void CDynEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if ((nChar >= '0' && nChar <= '9')||
		(nChar >= 'a' && nChar <= 'f')||
		(nChar >= 'A' && nChar <= 'F')||
		nChar == VK_BACK ||
		nChar == VK_DELETE)
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	
}
