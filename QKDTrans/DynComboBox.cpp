// DynComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "QKDTrans.h"
#include "DynComboBox.h"
#include "DlgAddCommand.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynComboBox

CDynComboBox::CDynComboBox(int id)
{
	m_id = id;
}

CDynComboBox::~CDynComboBox()
{
}


BEGIN_MESSAGE_MAP(CDynComboBox, CComboBox)
	//{{AFX_MSG_MAP(CDynComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynComboBox message handlers

