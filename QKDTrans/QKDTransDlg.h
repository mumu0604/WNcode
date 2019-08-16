
// QKDTransDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "DlgCommandSheet.h"
#include "DlgRefreshSheet.h"

// CQKDTransDlg dialog
class CQKDTransDlg : public CDialogEx
{
// Construction
public:
	CQKDTransDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_QKDTRANS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabQKDTrans;
	CDlgRefreshSheet m_CDlgRefreshSheet;
	CDlgCommandSheet m_CDlgCommandSheet;
	CDialog* m_TavPDialog[2];
	int m_CurSelTab;
	afx_msg void OnTcnSelchangeTabQkdtrans(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
