#pragma once


// CDlgRefreshSheet dialog

class CDlgRefreshSheet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRefreshSheet)

public:
	CDlgRefreshSheet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRefreshSheet();

// Dialog Data
	enum { IDD = IDD_DLGREFRESHSHEET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
