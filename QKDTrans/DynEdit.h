#if !defined(AFX_DYNEDIT_H__F09F267B_D66E_4B56_A518_2EF727A47090__INCLUDED_)
#define AFX_DYNEDIT_H__F09F267B_D66E_4B56_A518_2EF727A47090__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DynEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDynEdit window

class CDynEdit : public CEdit
{
// Construction
public:
	CDynEdit(int id);

// Attributes
public:

// Operations
public:
	CDynEdit(int id, int bitLength, double coef);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDynEdit();

	// Generated message map functions
protected:
	int m_id;
	double m_coef;
	int m_bitLength;
	//{{AFX_MSG(CDynEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNEDIT_H__F09F267B_D66E_4B56_A518_2EF727A47090__INCLUDED_)
