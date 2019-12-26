#pragma once
#include "afxcmn.h"
#include "Common.h"	// Added by ClassView
#include <vector>
#include<queue>
#include <stack>
#include <list>
//#include "DlgCommandSheet.h"
// CDlgRefreshSheet dialog
using namespace std;
typedef struct Rect2
{
public:
	int Id;
	double scale[4];
}control_2;
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
public:
	virtual BOOL OnInitDialog();
	vector<CString> m_InputNum;
	CListCtrl m_listcontrl_KZTELE;
	void displayList(bool isFirst, CmdInfo *m_pCmdInfo_Recv[256], int MonitorCmdNum);
	void MonitorDisplay(CmdInfo *pCmdInfo, int listCurrentNum, bool isFirst);
//	CDlgCommandSheet m_DlgCommandsheet;
	void ExtractArgValue(unsigned char *pDst, unsigned char *pSrc, int bitStart, int length);
	void operation(CString TBuff, vector<CString> & m_InputNum);
	double calculation(vector<CString> m_InputNum);
	CString Caculate(CString textbuff);
	int m_TotalTeleNUM;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void get_control_original_proportion();
	CRect m_rect;
	list<control_2*> m_con_list;
};
