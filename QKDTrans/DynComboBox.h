#if !defined(AFX_DYNCOMBOBOX_H__DC90C311_227B_4FFC_8944_8CA3CC9A9350__INCLUDED_)
#define AFX_DYNCOMBOBOX_H__DC90C311_227B_4FFC_8944_8CA3CC9A9350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DynComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDynComboBox window

class CDynComboBox : public CComboBox
{
// Construction
public:
	CDynComboBox(int id);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_id;
	virtual ~CDynComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDynComboBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNCOMBOBOX_H__DC90C311_227B_4FFC_8944_8CA3CC9A9350__INCLUDED_)
