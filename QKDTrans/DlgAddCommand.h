#pragma once
#include "Common.h"	// Added by ClassView
#include "Xml.h"
#include "libxml/parser.h"
#include "libxml/xmlmemory.h"
#include "afxcmn.h"
#include "DynEdit.h"
#include <vector>
#include "DynComboBox.h"
#include "afxdtctl.h"
#include "DlgCommandSheet.h"
// CDlgAddCommand dialog
enum{
	CANVAS_LEFTTOPx = 250,
	CANVAS_LEFTTOPy = 40,
	CANVAS_WIDTH = 700,
	CANVAS_HEIGHT = 500,
	LINEHEIGHT = 25,
	TABHEIGHT = 3,
	COMBOBOXWIDTH = 300,
	COMBOBOXHEIGHT = 500,
	TEXTWIDTH = 300,
	EDITWIDTH = 300,
};

enum{
	argType_input = 0,
	argType_select = 1,
	argType_QKDTime = 2
};

class CDlgAddCommand : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAddCommand)

public:
	CDlgAddCommand(CDlgCommandSheet *pDlg, CMD_WN *pCmd, int bNew, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAddCommand();
	bool m_bHaveChosenCmd;
	bool m_bIsAddNewCmd;
	CMD *m_pCMD;
	CMD_WN* m_pCmd_WN;
	int		m_bus_select;
	BOOL	m_bTimeFlag;
//	CDateTimeCtrl	m_timePick;
	int m_max_sec, m_min_sec, m_sec;
	CRect m_rect;
	CXML m_xml;
	int m_iArgCnt;
	int m_iArgByteLen[20];
	int m_iArgType[20];
	CDlgCommandSheet *m_pDlg;
//	CDateTimeCtrl	m_timePicker;
// Dialog Data
	enum { IDD = IDD_DLGADDCOMMAND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrlInCommand;
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	void DeleteEditAndCombo();
	void InitUI(unsigned char *pCmd);
	void SetValueToUI(unsigned char cmd_id, unsigned char *pArgValue);
	CDynEdit* CreatMyEdit(int id, int width, CString text, bool canEdit, int bitLength, double coef);
	CDynComboBox* CreateComboBox(int id, int width, xmlNodePtr pNode);
	void MoveNextLine();

	xmlXPathObjectPtr LocateXPath1(char xpath_expr[]);
	xmlXPathObjectPtr LocateCommand1(unsigned char dev_id, unsigned char cmd_id);
private:
	std::vector<CDynEdit*> m_vecEdit;
	std::vector<CDynComboBox*> m_vecCombobox;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonok();
	CDateTimeCtrl m_timePicker;
	int m_iTimeDay;
	int m_iTimeMiliSec;
	BOOL m_CheckTimeFlag;
};
