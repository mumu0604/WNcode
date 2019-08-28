// DlgCommandSheet.cpp : implementation file
//

#include "stdafx.h"
#include "QKDTrans.h"
#include "DlgCommandSheet.h"
#include "afxdialogex.h"
#include "DlgAddCommand.h"
#include "DlgRefreshSheet.h"
#include "QKDTimer.h"
#include <io.h>>
#include <direct.h>


// CDlgCommandSheet dialog

IMPLEMENT_DYNAMIC(CDlgCommandSheet, CDialogEx)
int m_COMportNum;

CInterface *m_pInterface;
int monitor_numcnt = 0;
struct RECVPARAM
{
	int portNum;
	HWND hwnd;
	CDlgCommandSheet *aa;
};
CDlgCommandSheet::CDlgCommandSheet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCommandSheet::IDD, pParent)
{
	m_lastIndirect = 0;
	m_iCmdStartIdx = 0;
	m_iRealCmdCnt = 0;
	GPSTimeNowday = m_Timer.GetGPSTimeNowday();
	m_displayMonitor = true;
}

CDlgCommandSheet::~CDlgCommandSheet()
{
}

void CDlgCommandSheet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrlCommand);
	DDX_Control(pDX, IDC_EDITCMDSEND, m_pEditCmdSend);
	DDX_Control(pDX, IDC_EDITCMDRECV, m_pEditCmdRecv);
	DDX_Control(pDX, IDC_LIST_MONITOR, m_listMonitor);
	DDX_Control(pDX, IDC_COMBO1, m_ComboPort);
}


BEGIN_MESSAGE_MAP(CDlgCommandSheet, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CDlgCommandSheet::OnDblclkList2)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST2, &CDlgCommandSheet::OnKeydownList2)
	ON_BN_CLICKED(IDC_BUTTON_IN_CMD, &CDlgCommandSheet::OnBnClickedButtonInCmd)
	ON_BN_CLICKED(IDC_BUTTON_OUT_CMD, &CDlgCommandSheet::OnBnClickedButtonOutCmd)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgCommandSheet::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_OPENCOM, &CDlgCommandSheet::OnBnClickedButtonOpencom)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CDlgCommandSheet::OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CDlgCommandSheet::OnBnClickedButtonWrite)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgCommandSheet message handlers

unsigned short CRC16Table[256];

void CRCTableGenerate(unsigned short h, unsigned char *crcTable)
{
	unsigned int i, j;
	unsigned char m = 16;
	unsigned int data;

	for (i = 0; i < 256; i++){
		data = i;
		for (j = m; j > 0; j--){
			if ((data & (1u << (m - 1))) == 0){
				data <<= 1;
			}
			else{
				data = (data << 1) ^ h;
			}
		}
		for (j = 0; j < m >> 3; j++){
			crcTable[(i << (m >> 4)) + j] = (unsigned char)((data & (0xFFu << (j << 3))) >> (j << 3) & 0xFFu);
		}
	}
}
BOOL CDlgCommandSheet::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_bus_flag = CAN_BUS_A;

	CRCTableGenerate(0x1021, (unsigned char *)CRC16Table);
	// TODO:  Add extra initialization here
	LONG lStyle;
	lStyle = GetWindowLong(m_ListCtrlCommand.m_hWnd, GWL_STYLE);
	lStyle &= ~LVS_TYPEMASK;
	lStyle |= LVS_REPORT;
	SetWindowLong(m_ListCtrlCommand.m_hWnd, GWL_STYLE, lStyle);

	DWORD dwStyle = m_ListCtrlCommand.GetExtendedStyle(); //获取当前扩展样式
	dwStyle |= LVS_EX_FULLROWSELECT; //选中某行使整行高亮（report风格时）
	dwStyle |= LVS_EX_GRIDLINES; //网格线（report风格时）
	//	dwStyle |= LVS_EX_CHECKBOXES; //item前生成checkbox控件
	m_ListCtrlCommand.SetExtendedStyle(dwStyle); //设置扩展风格

	typedef struct {
		char text[20];
		int width;
	}LIST_HEAD;

	LIST_HEAD head[] = {
		{ "序号", 36 },
		{ "分系统", 60 },
		{ "总线", 40 },
		{ "绝对时间", 180 },
		{ "指令功能描述", 200 },
		{ "指令码", 50 },
		{ "参数长度", 60 },
		{ "参数", 660 }
	};
	LV_COLUMN lvc[sizeof(head) / sizeof(LIST_HEAD)];
	int i;
	for (i = 0; i < sizeof(head) / sizeof(LIST_HEAD); i++)
	{
		lvc[i].mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
		lvc[i].iSubItem = 0;
		lvc[i].pszText = head[i].text;
		lvc[i].cx = head[i].width;
		m_ListCtrlCommand.InsertColumn(i, &lvc[i]);
	}
	m_xml.Open("commands.xml");
	GetCmdInfo(m_pCmdInfo);

	SYSTEMTIME t;
	GetSystemTime(&t);
	t.wHour = 0;
	t.wMinute = 0;
	t.wSecond = 0;
	t.wMilliseconds = 0;
	SystemTimeToFileTime(&t, (FILETIME *)&m_base_abs);
	SystemTimeToFileTime(&t, (FILETIME *)&m_base_relative);
	m_MonitorCmdNum = 0;

	
	SetTimer(1, 1000, NULL);
	m_pInterface = new CInterface;
	m_display.Open("monitor.xml", NULL, &m_listMonitor, NULL);
//	m_listMonitor.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES | LVS_EX_TRACKSELECT);
	m_iRealCmdCnt = 0;
	m_CMD_length = 0;
	m_CMDBuf = (char *)malloc(32 * 1024 + 8);

	

	#define NAME_LEN 100
	char szValueName[NAME_LEN];
	BYTE szPortName[NAME_LEN];
	LONG status=0;
	DWORD dwIndex = 0;
	DWORD dwSizeValueName = 100;
	DWORD dwSizeofPortName = 100;
	DWORD Type;
	dwSizeValueName = NAME_LEN;
	dwSizeofPortName = NAME_LEN;
	HKEY hKey;
	LPCTSTR lpSubKey = "HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return 0;
	}
	while (status != ERROR_NO_MORE_ITEMS)
	{
		status = RegEnumValue(hKey, dwIndex++, szValueName, &dwSizeValueName, NULL, &Type, szPortName, &dwSizeofPortName);
		if ((status == ERROR_SUCCESS))
		{
			m_ComboPort.AddString((char *)szPortName);

		}
		dwSizeValueName = NAME_LEN;
		dwSizeofPortName = NAME_LEN;
		
	}
	m_ComboPort.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}




void CDlgCommandSheet::OnDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	int iListIndex = m_ListCtrlCommand.GetSelectionMark();//焦点所在行数
	
	int iAimPosition = 0;
	CMD_WN cmd;
	CMD_WN *pCmd, *pCmd1;
	
	if (iListIndex == -1)//新建指令
	{
		CDlgAddCommand dlgAddCommand(this, &cmd, 1);
	
		dlgAddCommand.m_pCMD = &m_CMDArray[m_iRealCmdCnt];
		dlgAddCommand.m_bIsAddNewCmd = true;//控制addCommandsdlg不显示某个指令
		dlgAddCommand.m_bHaveChosenCmd = false;//
		
		if (dlgAddCommand.DoModal() == IDOK)
		{			
			cmd.dev_id = 0x33;
			AddCmdToList(&cmd, m_iRealCmdCnt, 1);
			m_iRealCmdCnt++;


		}

	}
	else//修改指令
	{
		CDlgAddCommand dlgAddCommand(this, &cmd, 1);
		dlgAddCommand.m_pCMD = &m_CMDArray[iListIndex];
		dlgAddCommand.m_bIsAddNewCmd = false;//控制addCommandsdlg显示m_CMDArray[iListIndex]指令
		dlgAddCommand.m_bHaveChosenCmd = true;
		CMD cmdTmp = m_CMDArray[iListIndex];//保存原来CMD指令
		if (dlgAddCommand.DoModal() == IDOK)
		{
			cmd.dev_id = 0x33;
			m_ListCtrlCommand.DeleteItem(iListIndex);
			AddCmdToList(&cmd, iListIndex, 1);
	//		m_iRealCmdCnt++;			
		}
		m_ListCtrlCommand.SetItemState(iListIndex, 0, -1);
	}

	*pResult = 0;
	m_ListCtrlCommand.SetSelectionMark(-1);
}
xmlXPathObjectPtr CDlgCommandSheet::LocateXPath(char xpath_expr[])
{
	xmlXPathObjectPtr xpathObj = NULL;
	xpathObj = m_xml.GetNodesByXPath(BAD_CAST(xpath_expr));//查询XPath表达式
	if (xpathObj){
		if (xpathObj->nodesetval){
			if (xpathObj->nodesetval->nodeNr > 0){
				return xpathObj;
			}
		}
		xmlXPathFreeObject(xpathObj);
	}
	return NULL;

}
xmlXPathObjectPtr CDlgCommandSheet::LocateCommand(unsigned char dev_id, unsigned char cmd_id)
{
	char xpath_expr[200];
	xmlXPathObjectPtr xpathObj = NULL;

	if (cmd_id){
		sprintf(xpath_expr, "/CommandSet/Category/Device[translate(@DeviceID,'abcdef','ABCDEF')='%02X']/parent::*/Command[translate(@CommandCode,'abcdef','ABCDEF')='%02X']\0", dev_id, cmd_id);
	}
	else{
		sprintf(xpath_expr, "/CommandSet/Category/Device[translate(@DeviceID,'abcdef','ABCDEF')='%02X']/parent::*/Command[@CommandCode]\0", dev_id);
	}
	return LocateXPath(xpath_expr);
}

void CDlgCommandSheet::InsertArgValue(unsigned char *pDst, unsigned char *pSrc, int bitStart, int length)
{
	int byteStart, bitEnd;

	unsigned char mask;
	int i;

	if (length <= 0){
		return;
	}

	byteStart = bitStart >> 3;


	bitStart = bitStart & 0x7;
	bitEnd = bitStart + length;

	if (!bitStart){
		for (i = 0; i < length / 8; i++){
			pDst[i + byteStart] = pSrc[i];
		}
		length &= 7;
		if (length){
			mask = 0xFF << (8 - length);
			pDst[i + byteStart] &= ~mask;
			pDst[i + byteStart] |= pSrc[i] & mask;
		}
		return;
	}

	mask = (0xFF >> bitStart);

	if (bitEnd < 8){
		mask &= 0xFF << (8 - bitEnd);
		pDst[byteStart] &= ~mask;
		pDst[byteStart] |= (pSrc[0] >> bitStart) & mask;
		return;
	}

	pDst[byteStart] &= ~mask;
	pDst[byteStart] |= (pSrc[0] >> bitStart) & mask;

	for (i = 1; i < bitEnd / 8; i++){
		pDst[i + byteStart] = (pSrc[i] >> bitStart) | (pSrc[i - 1] << (8 - bitStart));
	}

	bitEnd &= 7;
	if (bitEnd){
		mask = 0xFF << (8 - bitEnd);
		pDst[i + byteStart] &= ~mask;
		if (bitEnd > bitStart){
			pDst[i + byteStart] |= (pSrc[i] >> bitStart) & mask;
		}
		pDst[i + byteStart] |= (pSrc[i - 1] << (8 - bitStart)) & mask;
	}
}
void CDlgCommandSheet::GetCmdInfo(CmdInfo *m_pCmdInfo[256])
{
	memset(m_pCmdInfo, 0, sizeof(CmdInfo *)* 256);
	xmlXPathObjectPtr xpathObj;
	char xpath_expr[200];
	int i, j;
	xmlNode *pNode, *pNodeSub;
	xmlChar *xmlRtn;
	char *endptr;
	int idx;
	int length;
	CmdInfo *pCmdInfo;
	CString strTemp;
	unsigned char temp[MAX_ARG_LENGTH];
	unsigned char edit_index, combo_index;
	int sumGB2312;

	sprintf(xpath_expr, "/CommandSet/Category/Command");
	xpathObj = LocateXPath(xpath_expr);//查询节点
	monitor_numcnt = xpathObj->nodesetval->nodeNr;
	if (xpathObj){
		for (i = 0; i < xpathObj->nodesetval->nodeNr; i++){
			pNode = xpathObj->nodesetval->nodeTab[i];
			xmlRtn = xmlGetProp(pNode, BAD_CAST("CommandCode"));
			idx = strtol((char *)xmlRtn, &endptr, 16);
			idx &= 0xFF;
			xmlFree(xmlRtn);
			pCmdInfo = (CmdInfo *)malloc(sizeof(CmdInfo));
			m_pCmdInfo[idx] = pCmdInfo;
			memset(pCmdInfo, 0, sizeof(CmdInfo));
			xmlRtn = CXML::xmlGetPropGBK(pNode, BAD_CAST("CommandName"));
			length = strlen((char *)xmlRtn);
			if (length > MAX_NAME_LENGTH){
				length = MAX_NAME_LENGTH - 1;
			}
			memcpy(pCmdInfo->cmd_name, xmlRtn, length);
			sumGB2312 = 0;
			for (j = 0; j < length; j++){
				if (pCmdInfo->cmd_name[j] & 0x80){
					sumGB2312++;
				}
			}
			pCmdInfo->cmd_name[length] = '\0';
			if (sumGB2312 & 0x01){
				pCmdInfo->cmd_name[length - 1] = '\0';
			}
			xmlFree(xmlRtn);
			pNode = pNode->xmlChildrenNode;
			idx = 0;
			edit_index = 1;
			combo_index = 0;
			int cnt = 0;
			while (pNode){
				if (idx > MAX_ARG_NUM){
					break;
				}
				cnt++;
				if (!xmlStrcmp(pNode->name, BAD_CAST("Arg"))){//确定节点是否为Arg
					pCmdInfo->input_type[idx] = 1;
					pNodeSub = pNode->xmlChildrenNode;
					while (pNodeSub){
						if (!xmlStrcmp(pNodeSub->name, BAD_CAST("ArgValue"))){
							pCmdInfo->input_type[idx] = 0;
							break;
						}
						pNodeSub = pNodeSub->next;
					}
					xmlRtn = xmlGetProp(pNode, BAD_CAST("bitStart"));
					pCmdInfo->bit_start[idx] = atoi((char *)xmlRtn);
					xmlFree(xmlRtn);
					xmlRtn = xmlGetProp(pNode, BAD_CAST("bitLength"));
					pCmdInfo->arg_length[idx] = atoi((char *)xmlRtn);
					xmlFree(xmlRtn);
					xmlRtn = CXML::xmlGetPropGBK(pNode, BAD_CAST("ArgumentName"));
					length = strlen((char *)xmlRtn);
					if (length > MAX_NAME_LENGTH){
						length = MAX_NAME_LENGTH - 1;
					}
					memcpy(pCmdInfo->arg_name[idx], xmlRtn, length);
					sumGB2312 = 0;
					for (j = 0; j < length; j++){
						if (pCmdInfo->arg_name[idx][j] & 0x80){
							sumGB2312++;
						}
					}
					pCmdInfo->arg_name[idx][length] = '\0';
					if (sumGB2312 & 0x01){
						pCmdInfo->arg_name[idx][length - 1] = '\0';
					}
					xmlFree(xmlRtn);
					if (pCmdInfo->bit_start[idx] + pCmdInfo->arg_length[idx] > MAX_ARG_LENGTH * 8){
						break;
					}
					pCmdInfo->arg_byte_num += pCmdInfo->arg_length[idx];
					xmlRtn = xmlGetProp(pNode, BAD_CAST("initValue"));
					strTemp = xmlRtn;
					xmlFree(xmlRtn);
					strTemp.GetBufferSetLength((pCmdInfo->arg_length[idx] + 7) / 8 * 2);
					for (j = 0; j < pCmdInfo->arg_length[idx] / 8; j++){
						temp[j] = strtol(strTemp.Mid(j * 2, 2), &endptr, 16);
					}
					if (pCmdInfo->arg_length[idx] & 0x7){
						temp[j] = strtol(strTemp.Mid(j * 2, 2), &endptr, 16);
						if (pCmdInfo->input_type[idx] == 0){
							temp[j] <<= (8 - (pCmdInfo->arg_length[idx] & 0x7));
						}
					}
					InsertArgValue(pCmdInfo->init_value, temp, pCmdInfo->bit_start[idx], pCmdInfo->arg_length[idx]);
					if (pCmdInfo->input_type[idx] == 0){
						pCmdInfo->input_ctrl_index[idx] = combo_index;
						combo_index++;
						edit_index += 2;
					}
					else{
						pCmdInfo->input_ctrl_index[idx] = edit_index;
						edit_index += 3;
					}
					idx++;
				}
				pNode = pNode->next;
			}
			pCmdInfo->arg_num = idx;
			pCmdInfo->arg_byte_num /= 8;
		}
		xmlXPathFreeObject(xpathObj);
	}
	CXML coef_xml;
	coef_xml.Open("coef.xml");
	for (i = 0; i < 256; i++){
		if (m_pCmdInfo[i]){
			for (j = 0; j < m_pCmdInfo[i]->arg_num; j++){
				m_pCmdInfo[i]->coef[j] = GetCoefficient(&coef_xml, i, m_pCmdInfo[i]->bit_start[j]);
			}
		}
	}
}
double CDlgCommandSheet::GetCoefficient(CXML *pXml, unsigned char cmd_id, int bitStart)
{
	double coef = 1.0;
	xmlXPathObjectPtr xpathObj;
	char xpath_expr[200];
	xmlChar *xmlRtn;

	sprintf(xpath_expr, "/Coefs/Coefficient[translate(@CommandCode,'abcdef','ABCDEF')='%02X' and @BitStart='%d']",
		cmd_id, bitStart);
	xpathObj = pXml->GetNodesByXPath(BAD_CAST(xpath_expr));
	if (!xpathObj){
		return coef;
	}
	if (!xpathObj->nodesetval){
		xmlXPathFreeObject(xpathObj);
		return coef;
	}
	if (xpathObj->nodesetval->nodeNr == 0){
		xmlXPathFreeObject(xpathObj);
		return coef;
	}

	xmlRtn = xmlNodeGetContent(xpathObj->nodesetval->nodeTab[0]);
	coef = atof((char *)xmlRtn);
	xmlFree(xmlRtn);
	return coef;
}
void CDlgCommandSheet::FillNote(CMD *pCMD)
{
	pCMD->Note = "";
	pCMD->SubNote = "";
	CString strIDBuf;
	strIDBuf.Format("%02X", pCMD->ID);

	xmlXPathObjectPtr xpathObj;
	xmlNodePtr pNode, pChild, pArgValue;
	xmlChar *xmlRtn;

	char xpath_expr[200];
	int inputType, byteLength;
	int iOffset = 0;

	sprintf(xpath_expr, "%s%s']\0", "//Command[@code='", strIDBuf);

	xpathObj = m_xml.GetNodesByXPath(BAD_CAST(xpath_expr));
	if (xpathObj){
		if (xpathObj->nodesetval->nodeNr == 0){
			xmlXPathFreeObject(xpathObj);
			return;
		}
		pNode = xpathObj->nodesetval->nodeTab[0];
		xmlRtn = xmlGetNodePath(pNode);
		if (!strstr((char *)xmlRtn, "HostCommand")){
			pCMD->IsLocal = false;
		}
		else{
			pCMD->IsLocal = true;
		}
		xmlFree(xmlRtn);
		xmlRtn = CXML::xmlGetPropGBK(pNode, BAD_CAST("name"));
		pCMD->Note = xmlRtn;
		xmlFree(xmlRtn);
		pChild = pNode->xmlChildrenNode;
		while (pChild != NULL){
			if (!xmlStrcmp(pChild->name, BAD_CAST("Arg"))){
				xmlRtn = xmlGetProp(pChild, BAD_CAST("inputType"));
				inputType = atoi((char *)xmlRtn);
				xmlFree(xmlRtn);
				xmlRtn = xmlGetProp(pChild, BAD_CAST("byteLength"));
				byteLength = atoi((char *)xmlRtn);
				xmlFree(xmlRtn);
				if (inputType == 0){
					pArgValue = pChild->xmlChildrenNode;
					CString argStr = "", tmpStr;
					for (int i = 0; i < byteLength; i++){
						tmpStr.Format("%02X", pCMD->Args[iOffset + i]);
						argStr += tmpStr;
					}
					while (pArgValue != NULL){
						if (!xmlStrcmp(pArgValue->name, BAD_CAST("ArgValue"))){
							xmlRtn = xmlGetProp(pArgValue, BAD_CAST("code"));
							tmpStr = xmlRtn;
							xmlFree(xmlRtn);
							if (tmpStr == argStr){
								xmlRtn = CXML::xmlGetPropGBK(pArgValue, BAD_CAST("name"));
								if (pCMD->SubNote != ""){
									pCMD->SubNote += ",";
								}
								pCMD->SubNote += (CString)xmlRtn;
								xmlFree(xmlRtn);
							}
						}
						pArgValue = pArgValue->next;
					}
				}
				iOffset += byteLength;
			}
			pChild = pChild->next;
		}
		xmlXPathFreeObject(xpathObj);
	}
	return;
}
int CDlgCommandSheet::FindPosition(bool up_direction, int rawPosition)
{
	int aimPosition;
	if (up_direction)
	{
		for (aimPosition = rawPosition - 1; aimPosition > -1; aimPosition--)//找到第一个比之小的
		{
			if (m_CMDArray[aimPosition].ShipTime <= m_CMDArray[rawPosition].ShipTime){
				break;
			}
		}
		return aimPosition + 1;
	}
	else
	{
		for (aimPosition = rawPosition + 1; aimPosition < m_iRealCmdCnt; aimPosition++)//找到第一个比之大的
		{
			if (m_CMDArray[aimPosition].ShipTime >= m_CMDArray[rawPosition].ShipTime){
				break;
			}
		}
		return aimPosition - 1;
	}
}
////////////////////////////////////////////////////////////////////////////将rawPosition的CMD调到destination位置，同时rawPositon后面的依次顺延
void CDlgCommandSheet::ResortCMD(int destination, int rawPosition)
{
	CMD cmdBuf;
	if (rawPosition == destination)//
		return;
	else if (rawPosition > destination)
	{
		cmdBuf = m_CMDArray[rawPosition];
		for (int i = rawPosition - 1; i >= destination; i--)
		{
			m_CMDArray[i + 1] = m_CMDArray[i];
		}
		m_CMDArray[destination] = cmdBuf;
	}
	else
	{
		cmdBuf = m_CMDArray[rawPosition];
		for (int i = rawPosition + 1; i <= destination; i++)
		{
			m_CMDArray[i - 1] = m_CMDArray[i];
		}
		m_CMDArray[destination] = cmdBuf;
	}
}
void CDlgCommandSheet::DisplayCmd(int index, CMD cmdBuf)
{
	CString strBuf;
	int iRow;
	strBuf.Format("%d", index + 1);
	iRow = m_ListCtrlCommand.InsertItem(index, strBuf);

	strBuf = cmdBuf.Note;
	if (cmdBuf.SubNote != ""){
		strBuf += "|";
		strBuf += cmdBuf.SubNote;
	}
	m_ListCtrlCommand.SetItemText(iRow, 1, strBuf);//pListCtrl->SetItemText(nRow, 2, strTime);

	CString strBuf1;
	strBuf.Format("%03d", QKDTimer::GetDayofShipTime(cmdBuf.ShipTime));
	strBuf += "日";
	strBuf1.Format("%02d", QKDTimer::GetHourofShipTime(cmdBuf.ShipTime));
	strBuf += strBuf1 + "时";
	strBuf1.Format("%02d", QKDTimer::GetMinuteofShipTime(cmdBuf.ShipTime));
	strBuf += strBuf1 + "分";
	strBuf1.Format("%02d", QKDTimer::GetSecondofShipTime(cmdBuf.ShipTime));
	strBuf += strBuf1 + "秒";
	strBuf1.Format("%03d", QKDTimer::GetMillisecondofShipTime(cmdBuf.ShipTime));
	strBuf += strBuf1 + "毫秒";
	m_ListCtrlCommand.SetItemText(iRow, 2, strBuf);

	strBuf = "0x";
	strBuf1.Format("%02X", cmdBuf.ID);
	strBuf += strBuf1;
	m_ListCtrlCommand.SetItemText(iRow, 3, strBuf);

	strBuf.Format("%d", cmdBuf.ArgByteLength);
	m_ListCtrlCommand.SetItemText(iRow, 4, strBuf);

	strBuf = "";
	strBuf1.Format("%02X", cmdBuf.Args[0]);
	strBuf += strBuf1;
	for (int i = 1; i < cmdBuf.ArgByteLength; i++)
	{
		strBuf1.Format("%02X", cmdBuf.Args[i]);
		strBuf = strBuf + " " + strBuf1;
	}
	m_ListCtrlCommand.SetItemText(iRow, 5, strBuf);
}
CString CDlgCommandSheet::GetDeviceName(unsigned char dev_id)
{
	xmlXPathObjectPtr xpathObj;
	char xpath_expr[200];
	CString strBuf;

	strBuf = "无效!";
	sprintf(xpath_expr, "/CommandSet/Category/Device[translate(@DeviceID,'abcdef','ABCDEF')='%02X']\0", dev_id);
	xpathObj = LocateXPath(xpath_expr);
	if (xpathObj){
		strBuf = CXML::xmlGetPropGBK(xpathObj->nodesetval->nodeTab[0], BAD_CAST("DeviceName"));
		xmlXPathFreeObject(xpathObj);
	}
	return strBuf;
}
__int64 CDlgCommandSheet::DiffTimeAbsReletive(void)
{
	__int64 diff;

	if (m_base_abs.QuadPart > m_base_relative.QuadPart){
		diff = m_base_abs.QuadPart - m_base_relative.QuadPart;
	}
	else{
		diff = m_base_relative.QuadPart - m_base_abs.QuadPart;
		diff = -diff;
	}
	diff = diff / SEC_PER_100ns;
	return diff;
}
HMSTime CDlgCommandSheet::SecondToHMSTime(int second)
{
	HMSTime time;
	time.hour = second / 3600;
	second %= 3600;
	time.minute = second / 60;
	time.second = second % 60;
	return time;
}
int CDlgCommandSheet::HMSTimeToSecond(HMSTime time)
{
	int second;
	second = time.hour * 3600 + time.minute * 60 + time.second;
	return second;
}
CString CDlgCommandSheet::GetAbsTime(unsigned int sec)
{
	SYSTEMTIME t;
	CString strBuf1, strBuf2;
	ULARGE_INTEGER base;

	base.QuadPart = m_base_abs.QuadPart + sec*SEC_PER_100ns;
	FileTimeToSystemTime((FILETIME *)&base, &t);

	strBuf1 = "";
	strBuf2.Format("%04d年", t.wYear);
	strBuf1 += strBuf2;

	strBuf2.Format("%02d月", t.wMonth);
	strBuf1 += strBuf2;
	strBuf2.Format("%02d日", t.wDay);
	strBuf1 += strBuf2;

	strBuf2.Format("%02d时%02d分%02d秒",
		t.wHour, t.wMinute, t.wSecond);
	strBuf1 += strBuf2;
	return strBuf1;
}
void CDlgCommandSheet::ExtractArgValue(unsigned char *pDst, unsigned char *pSrc, int bitStart, int length)
{
	int i;

	int byteStart = bitStart / 8;

	bitStart &= 7;

	if (!bitStart){
		for (i = 0; i < length / 8; i++){
			pDst[i] = pSrc[i + byteStart];
		}
		length &= 7;
		if (length){
			pDst[i] = pSrc[i + byteStart] & (0xFF << (8 - length));
		}
		return;
	}

	for (i = 0; i < length / 8; i++){
		pDst[i] = (pSrc[i + byteStart] << bitStart) | pSrc[i + byteStart + 1] >> (8 - bitStart);
	}
	length &= 7;
	if (length){
		pDst[i] = (pSrc[i + byteStart] << bitStart);
		if (length + bitStart > 8){
			pDst[i] |= pSrc[i + byteStart + 1] >> (8 - bitStart);
		}
		pDst[i] &= (0xFF << (8 - length));
	}
}
void CDlgCommandSheet::AddCmdToList(CMD_WN *pCmd, int index, int bNew)
{
	int i, j, k;
	int iRow;
	CString strBuf, strBuf1;

	CString strDev;

	CmdInfo *pCmdInfo;
	CMD_WN *pAddedCmd;

	unsigned char temp[MAX_ARG_LENGTH];

	strDev = GetDeviceName(pCmd->dev_id);
	if (!strDev.Compare("无效!")){
		MessageBox("设备不存在,指令插入/替换失败!", "警告");
		return;
	}
	pCmdInfo = m_pCmdInfo[pCmd->cmd_id];
	if (!pCmdInfo){
		MessageBox("指令不存在,指令插入/替换失败!", "警告");
		return;
	}

	iRow = m_ListCtrlCommand.GetItemCount();
// 	if (index >= iRow){
// 		MessageBox("指令定位错误,指令插入/替换失败!", "警告");
// 		return;
// 	}
	if (index < 0){
		index = iRow;
	}
	pAddedCmd = (CMD_WN *)malloc(sizeof(CMD_WN));
	memcpy(pAddedCmd, pCmd, sizeof(CMD_WN));
	if (!bNew){
		free((CMD *)m_ListCtrlCommand.GetItemData(index));
	}
	else{
		m_ListCtrlCommand.InsertItem(index, "");
		for (i = index; i < m_ListCtrlCommand.GetItemCount(); i++){
			strBuf.Format("%0d", i);
			m_ListCtrlCommand.SetItemText(i, COL_SEQ, strBuf);
		}
	}
	m_ListCtrlCommand.SetItemData(index, (DWORD)pAddedCmd);

	m_ListCtrlCommand.SetItemText(index, COL_DEV, strDev);

	if (pAddedCmd->bus_flag){
		m_ListCtrlCommand.SetItemText(index, COL_BUS, "B");
	}
	else{
		m_ListCtrlCommand.SetItemText(index, COL_BUS, "A");
	}
	if (pAddedCmd->immediate_flag){
		m_ListCtrlCommand.SetItemText(index, COL_ABS_TIME, "立即令");	
	}
	else{
		m_ListCtrlCommand.SetItemText(index, COL_ABS_TIME, GetAbsTime(pAddedCmd->time));		
	}

	m_ListCtrlCommand.SetItemText(index, COL_DESC, (char *)pCmdInfo->cmd_name);

	strBuf.Format("%02X", pAddedCmd->cmd_id);
	m_ListCtrlCommand.SetItemText(index, COL_CODE, strBuf);

	strBuf.Format("%02d", pCmdInfo->arg_byte_num);
	m_ListCtrlCommand.SetItemText(index, COL_ARG_LEN, strBuf);

	strBuf = "";
	for (j = 0; j < pCmdInfo->arg_num; j++){
		ExtractArgValue(temp, pAddedCmd->args, pCmdInfo->bit_start[j], pCmdInfo->arg_length[j]);
		for (k = 0; k < pCmdInfo->arg_length[j] / 8; k++){
			strBuf1.Format("%02X", temp[k]);
			strBuf += strBuf1;
		}
		if (pCmdInfo->arg_length[j] & 0x7){
			if (pCmdInfo->input_type[j] == 0){
				temp[k] >>= (8 - (pCmdInfo->arg_length[j] & 0x7));
			}
			strBuf1.Format("%02X", temp[k]);
			strBuf += strBuf1;
		}
		strBuf += " ";
	}
	m_ListCtrlCommand.SetItemText(index, COL_ARG, strBuf);

	LV_COLUMN lvc;
	lvc.mask = LVCF_TEXT;

	lvc.pszText = _T("绝对时间");
	m_ListCtrlCommand.SetColumn(COL_ABS_TIME, &lvc);
	lvc.pszText = _T("分系统");
	m_ListCtrlCommand.SetColumn(COL_DEV, &lvc);
//	m_ListCtrlCommand.SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	return;
}
void CDlgCommandSheet::OnKeydownList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	int i = 0, iListIndex = -1;
	CString strTxt;
	switch (pLVKeyDow->wVKey)
	{
	case VK_DELETE:
		iListIndex = m_ListCtrlCommand.GetSelectionMark();
		if (!m_ListCtrlCommand.GetItemState(iListIndex, LVIS_FOCUSED)){
			MessageBox("请先选择需删除的指令！", "错误");
			return;
		}
		m_iRealCmdCnt -= 1;
		CMD_WN *pCmd;
		pCmd = (CMD_WN *)m_ListCtrlCommand.GetItemData(iListIndex);
		m_ListCtrlCommand.DeleteItem(iListIndex);
	
		for (int i = iListIndex; i < m_ListCtrlCommand.GetItemCount(); i++){
			strTxt.Format("%d", i);
			m_ListCtrlCommand.SetItemText(i, COL_SEQ, strTxt);
		}
		m_bCmdChanged = true;
		break;
	case 'S':
		OnSend();
		m_pInterface->SendCmd(m_COMportNum, 0, 32, (unsigned char *)m_CMDBuf, false);
		break;
	case 'D':
		m_iRealCmdCnt = 0;
		m_ListCtrlCommand.DeleteAllItems();
		break;
	}
	*pResult = 0;
	m_ListCtrlCommand.SetSelectionMark(-1);
}

unsigned short CRC16(unsigned char *pDataToCRC, unsigned short crc_init, int length_crc)
{
	unsigned short crc = crc_init;
	unsigned short crc_tmp;
	unsigned char da;
	volatile int i;

	for (i = 0; i < length_crc; i++){
		da = (unsigned char)(crc >> 8);
		crc_tmp = (unsigned short)(crc << 8);
		crc = (unsigned short)(CRC16Table[da^ pDataToCRC[i]] ^ crc_tmp);
	}
	return(crc);
}
void CDlgCommandSheet::SaveToPLD(CFile *pldFile)
{
	int i, j;
	CMD_WN *pCmd;
	CmdInfo *pCmdInfo;
	int cmdTimeFlag;

	char *pBuf, *pBuf1;
	int cnt;

	int idx32B = 0, eventNum32B = 0, dev32B, bus32B, left32B = 0, time32B, byte32B;//数据注入段采用32字节
	//指令类型：1字节，指令序列计数：1字节；数据注入段顺序、AB总线标志：1字节；事件标识：1字节；数据：28字节
	int cnt32BDev[4] = { 0, 0, 0, 0 }, start32BDev[4] = { 0, 0, 0, 0 }, end32BDev[4] = { 0, 0, 0, 0 };
	int devIndex;

	unsigned int time;

	unsigned char sum;
	char xw_header[8];

	pBuf1 = (char *)malloc(32 * 1024 + 8);
	pBuf = pBuf1;

	cnt = m_ListCtrlCommand.GetItemCount();
	if (cnt > 1024){
		cnt = 1024;
	}
	eventNum32B = 0;
	idx32B = 0;
	char *buffer;
	char buffer1[64];
	for (i = 0; i < cnt; i++){
		buffer = (char*)m_ListCtrlCommand.GetItemData(i);
		memcpy(buffer1, buffer, 40);
		pCmd = (CMD_WN *)m_ListCtrlCommand.GetItemData(i);
		pCmdInfo = m_pCmdInfo[pCmd->cmd_id & 0xFF];
		if (!pCmdInfo){
			MessageBox("无法读取指令定义信息，导出到注入文件失败！", "错误");
			free(pBuf1);
			return;
		}
		cmdTimeFlag = 1;
		if (pCmd->immediate_flag){
			cmdTimeFlag = 0;
		}

		if ((left32B < pCmdInfo->arg_byte_num + 5) || //32B full
			(pCmd->dev_id != dev32B) || //device changed
			(pCmd->bus_flag != bus32B) || //bus changed
			(cmdTimeFlag != time32B)){	//time flag changed
			//1.output old 32B
			if (eventNum32B > 0){
				pBuf[0] = dev32B;
				devIndex = ((dev32B & 0x80) >> 6) | ((dev32B & 0x20) >> 5);//a simple hack for retrieving device index.
				if ((dev32B == DEV_ID_KZ) && (time32B == 0)){
					pBuf[1] = 0;
					pBuf[2] = bus32B;
				}
				else{
					pBuf[1] = cnt32BDev[devIndex];
					if (!cnt32BDev[devIndex]){
						start32BDev[devIndex] = idx32B;
					}
					end32BDev[devIndex] = idx32B;
					cnt32BDev[devIndex] ++;
					pBuf[2] = bus32B | 0xC;
				}
				pBuf[3] = eventNum32B;
				if (time32B){
					pBuf[3] |= 0xF0;
				}
				memset(pBuf + byte32B, 0x5A, left32B);
				idx32B++;
				pBuf += 32;
			}

			//2. new 32B
			eventNum32B = 0;
			dev32B = pCmd->dev_id;
			bus32B = pCmd->bus_flag;
			left32B = 27;
			byte32B = 4;
			time32B = cmdTimeFlag;
		}
		time = 0;
		if (time32B){
			time = pCmd->time;
		}
		pBuf[byte32B] = (time & 0xFF000000) >> 24;
		pBuf[byte32B + 1] = (time & 0xFF0000) >> 16;
		pBuf[byte32B + 2] = (time & 0xFF00) >> 8;
		pBuf[byte32B + 3] = time & 0xFF;
		pBuf[byte32B + 4] = pCmd->cmd_id;
		memcpy(pBuf + byte32B + 5, pCmd->args, pCmdInfo->arg_byte_num);
		byte32B += pCmdInfo->arg_byte_num + 5;
		left32B -= pCmdInfo->arg_byte_num + 5;
		eventNum32B++;
	}

	pBuf[0] = dev32B;
	devIndex = ((dev32B & 0x80) >> 6) | ((dev32B & 0x20) >> 5);//a simple hack for retrieving device index.
	if ((dev32B == DEV_ID_KZ) && (time32B == 0)){
		pBuf[1] = 0;
		pBuf[2] = bus32B;
	}
	else{
		pBuf[1] = cnt32BDev[devIndex];
		if (!cnt32BDev[devIndex]){
			start32BDev[devIndex] = idx32B;
		}
		end32BDev[devIndex] = idx32B;
		cnt32BDev[devIndex] ++;
		pBuf[2] = bus32B | 0xC;
	}
	pBuf[3] = eventNum32B;
	if (time32B){
		pBuf[3] |= 0xF0;
	}

	memset(pBuf + byte32B, 0x5A, left32B);
	idx32B++;

	for (i = 0; i < 4; i++){
		if (!cnt32BDev[i]){
			continue;
		}
		if (cnt32BDev[i] == 1){
			pBuf1[2 + start32BDev[i] * 32] = pBuf1[2 + start32BDev[i] * 32] & 0xF0;
		}
		else{
			pBuf1[2 + start32BDev[i] * 32] = (pBuf1[2 + start32BDev[i] * 32] & 0xF0) | 0x03;
			pBuf1[2 + end32BDev[i] * 32] = (pBuf1[2 + end32BDev[i] * 32] & 0xF0) | 0x0F;
		}
	}

	pBuf = pBuf1;
	for (i = 0; i < idx32B; i++){
		sum = 0;
		for (j = 0; j < 31; j++){
			sum += pBuf[j];
		}
		pBuf[31] = sum;
		pBuf += 32;
	}


	unsigned short crc;
	pBuf = pBuf1;
	crc = CRC16((unsigned char *)pBuf, 0, idx32B * 32);
// 	pBuf[idx32B * 32] = (crc & 0xFF00) >> 8;
// 	pBuf[idx32B * 32 + 1] = crc & 0xFF;


	i = idx32B * 32 + 2;
	xw_header[0] = (i & 0xFF000000) >> 24;
	xw_header[1] = (i & 0xFF0000) >> 16;
	xw_header[2] = (i & 0xFF00) >> 8;
	xw_header[3] = i & 0xFF;
	xw_header[4] = 0x01;
	xw_header[5] = 0x24;
	crc = 0;
	for (i = 0; i < idx32B * 32 + 2; i++){
		crc += (unsigned char)(pBuf[i]);
	}
	xw_header[6] = (crc & 0xFF00) >> 8;
	xw_header[7] = crc & 0xFF;

//	pldFile->Write(xw_header, 8);
//	pldFile->Write(pBuf, idx32B * 32 + 2);
	pldFile->Write(pBuf, idx32B * 32);
	memcpy(m_CMDBuf, pBuf, idx32B * 32);
	m_CMD_length = idx32B * 32 + 2;
	free(pBuf1);
}
void CDlgCommandSheet::DeleteCmdItem(int iIdx)
{
	if (iIdx < 0)
		return;
	int i;
	for (i = iIdx; i < m_iRealCmdCnt - 1; i++)
	{
		m_CMDArray[i] = m_CMDArray[i + 1];
	}

	m_iRealCmdCnt -= 1;
}
void CDlgCommandSheet::FillCmdSendEdit(CMD *pCMD)
{
	m_pEditCmdSend.SetSel(-1, -1);
	CString strContent = "\r\n", strBuf;
	strBuf.Format("%d", m_iCmdSendCnt);
	strContent += strBuf + ":";
	strContent += pCMD->Note;
	if (pCMD->SubNote != ""){
		strContent += "|" + pCMD->SubNote;
	}
	m_pEditCmdSend.ReplaceSel(strContent);
}
void CDlgCommandSheet::OnSend()
{
	// TODO: Add your control notification handler code here
	int cmdIdx = m_ListCtrlCommand.GetSelectionMark();
	if (cmdIdx < 0)
	{
		MessageBox("未选定指令！");
		return;
	}
	int clickRelt = MessageBox("是否发送该指令?", "提示", MB_OK | MB_OKCANCEL);
	if (MB_OKCANCEL == clickRelt)
	{
		
		int i, j;
		CMD_WN *pCmd;
		CmdInfo *pCmdInfo;
		int cmdTimeFlag;

		char *pBuf, *pBuf1;
		int cnt;
		int idx32B = 0, eventNum32B = 0, dev32B, bus32B, left32B = 0, time32B, byte32B;//数据注入段采用32字节
		//指令类型：1字节，指令序列计数：1字节；数据注入段顺序、AB总线标志：1字节；事件标识：1字节；数据：28字节
		int cnt32BDev[4] = { 0, 0, 0, 0 }, start32BDev[4] = { 0, 0, 0, 0 }, end32BDev[4] = { 0, 0, 0, 0 };
		int devIndex;

		unsigned int time;

		unsigned char sum;
		char xw_header[8];
		pBuf1 = (char *)malloc(32 * 1024 + 8);
		pBuf = pBuf1;

		cnt = m_ListCtrlCommand.GetItemCount();
		eventNum32B = 0;
		idx32B = 0;
		char *buffer;
		char buffer1[64];
		buffer = (char*)m_ListCtrlCommand.GetItemData(cmdIdx);
		memcpy(buffer1, buffer, 40);
		pCmd = (CMD_WN *)m_ListCtrlCommand.GetItemData(cmdIdx);
		pCmdInfo = m_pCmdInfo[pCmd->cmd_id & 0xFF];
		cmdTimeFlag = 1;
		if (pCmd->immediate_flag){
			cmdTimeFlag = 0;
		}
		eventNum32B = 0;
		dev32B = pCmd->dev_id;
		bus32B = pCmd->bus_flag;
		left32B = 27;
		byte32B = 4;
		time32B = cmdTimeFlag;
		time = 0;
		if (time32B){
			time = pCmd->time;
		}
		pBuf[byte32B] = (time & 0xFF000000) >> 24;
		pBuf[byte32B + 1] = (time & 0xFF0000) >> 16;
		pBuf[byte32B + 2] = (time & 0xFF00) >> 8;
		pBuf[byte32B + 3] = time & 0xFF;
		pBuf[byte32B + 4] = pCmd->cmd_id;
		memcpy(pBuf + byte32B + 5, pCmd->args, pCmdInfo->arg_byte_num);
		byte32B += pCmdInfo->arg_byte_num + 5;
		left32B -= pCmdInfo->arg_byte_num + 5;
		eventNum32B++;

		pBuf[0] = dev32B;
		devIndex = ((dev32B & 0x80) >> 6) | ((dev32B & 0x20) >> 5);//a simple hack for retrieving device index.
		if ((dev32B == DEV_ID_KZ) && (time32B == 0)){
			pBuf[1] = 0;
			pBuf[2] = bus32B;
		}
		else{
			pBuf[1] = cnt32BDev[devIndex];
			if (!cnt32BDev[devIndex]){
				start32BDev[devIndex] = idx32B;
			}
			end32BDev[devIndex] = idx32B;
			cnt32BDev[devIndex] ++;
			pBuf[2] = bus32B | 0xC;
		}
		pBuf[3] = eventNum32B;
		if (time32B){
			pBuf[3] |= 0xF0;
		}

		memset(pBuf + byte32B, 0x5A, left32B);
		idx32B++;

		for (i = 0; i < 4; i++){
			if (!cnt32BDev[i]){
				continue;
			}
			if (cnt32BDev[i] == 1){
				pBuf1[2 + start32BDev[i] * 32] = pBuf1[2 + start32BDev[i] * 32] & 0xF0;
			}
			else{
				pBuf1[2 + start32BDev[i] * 32] = (pBuf1[2 + start32BDev[i] * 32] & 0xF0) | 0x03;
				pBuf1[2 + end32BDev[i] * 32] = (pBuf1[2 + end32BDev[i] * 32] & 0xF0) | 0x0F;
			}
		}

		pBuf = pBuf1;
		for (i = 0; i < idx32B; i++){
			sum = 0;
			for (j = 0; j < 31; j++){
				sum += pBuf[j];
			}
			pBuf[31] = sum;
			pBuf += 32;
		}
		pBuf = pBuf1;
		memcpy(m_CMDBuf, pBuf, idx32B * 32);
		m_CMD_length = idx32B * 32 + 2;

		m_iCmdSendCnt++;
		CString strCnt;
		strCnt.Format("%d", m_iCmdSendCnt);
		GetDlgItem(IDC_EDITCMDSENDCNT)->SetWindowText(strCnt);
		m_ListCtrlCommand.SetFocus();

	}
}
void CDlgCommandSheet::SendCycleCmd()
{
	UpdateData(TRUE);
	int frameByteLength = 0;
	int byte_received = 0;
	CString strBuf;
	__int64 ship_time, gps_time;

	ship_time = m_Timer.GetGPSTime() - GPSTimeNowday;

	while (TRUE){
		if (ship_time - m_lastIndirect < 100){
			break;
		}
		if (m_iCmdStartIdx >= m_iRealCmdCnt){
			break;
		}
		if (m_CMDArray[m_iCmdStartIdx].ShipTime > ship_time){
			break;
		}
		if (m_CMDArray[m_iCmdStartIdx].ShipTime + 3000 < ship_time){
			m_iCmdStartIdx++;
			break;
		}
		unsigned char *pBuf = (unsigned char*)malloc(m_CMDArray[m_iCmdStartIdx].ArgByteLength + 1);
		*pBuf = m_CMDArray[m_iCmdStartIdx].ID;
		memcpy(pBuf + 1, m_CMDArray[m_iCmdStartIdx].Args, m_CMDArray[m_iCmdStartIdx].ArgByteLength);

		m_pInterface->SendCmd(0,cmdType_indirect, m_CMDArray[m_iCmdStartIdx].ArgByteLength + 1, pBuf, m_CMDArray[m_iCmdStartIdx].IsLocal);
		FillCmdSendEdit(&m_CMDArray[m_iCmdStartIdx]);
		m_iCmdSendCnt++;
		CString strCnt;
		strCnt.Format("%d", m_iCmdSendCnt);
		GetDlgItem(IDC_EDITCMDSENDCNT)->SetWindowText(strCnt);

		m_iCmdStartIdx++;
		free(pBuf);
		m_lastIndirect = ship_time;
		break;
	}

	strBuf.Format("%03d日%02d时%02d分%02d秒%03d毫秒",
		m_Timer.GetDayofShipTime(ship_time),
		m_Timer.GetHourofShipTime(ship_time),
		m_Timer.GetMinuteofShipTime(ship_time),
		m_Timer.GetSecondofShipTime(ship_time),
		m_Timer.GetMillisecondofShipTime(ship_time));

	GetDlgItem(IDC_BUTTON_TIME)->SetWindowText(strBuf);

// 	strBuf.Format("%03d日%02d时%02d分%02d秒%03d毫秒",
// 		QKDTimer::GetDayofShipTime(ship_time),
// 		QKDTimer::GetHourofShipTime(ship_time),
// 		QKDTimer::GetMinuteofShipTime(ship_time),
// 		QKDTimer::GetSecondofShipTime(ship_time),
// 		QKDTimer::GetMillisecondofShipTime(ship_time));
// 
// 	GetDlgItem(IDDSHIPTIME)->SetWindowText(strBuf);
// 
// 	switch (m_pInterface->m_LVDSLast)
// 	{
// 	case 0:
// 		GetDlgItem(IDC_LVDSSTATUS)->SetWindowText("接收");
// 		break;
// 	case 1:
// 		GetDlgItem(IDC_LVDSSTATUS)->SetWindowText("发送");
// 		break;
// 	default:
// 		GetDlgItem(IDC_LVDSSTATUS)->SetWindowText("");
// 		break;
// 	}
// 	strBuf.Format("%d", m_pInterface->m_LVDSFrameCntExpected);
// 	GetDlgItem(IDC_LVDSCntShould)->SetWindowText(strBuf);
// 	strBuf.Format("%d", m_pInterface->m_LVDSFrameCnt);
// 	GetDlgItem(IDC_LVDSCntReal)->SetWindowText(strBuf);

}
void CDlgCommandSheet::LoadFromPLD(CString fileName)
{
	CFile pldFile;
	pldFile.Open(fileName, CFile::modeRead | CFile::typeBinary);
	CMD_WN cmd;
	CmdInfo *pCmdInfo;

	unsigned char buf[32];
	int cnt, idx = 0, iPos;

	cnt = pldFile.GetLength();
// 	if ((cnt & 0x1F) != 10){
// 		return;
// 	}

	cnt >>= 5;
//	pldFile.Read(buf, 8);//度xw_header[8]
	unsigned short crc = 0;

// 	for (idx = 0; idx < cnt; idx++){
// 		pldFile.Read(buf, 32);
// 		crc = CRC16((unsigned char *)buf, crc, 32);
// 	}
// 	pldFile.Read(buf, 2);
// 	if ((((crc & 0xFF00) >> 8) != buf[0]) || ((crc & 0xFF) != buf[1])){
// 		MessageBox("文件格式错误！");
// 		return;
// 	}

	pldFile.SeekToBegin();
//	pldFile.Read(buf, 8);



	for (int i = 0; i < cnt; i++){
		pldFile.Read(buf, 32);
		iPos = 4;
		for (int j = 0; j < (buf[3] & 0xF); j++){
			cmd.dev_id = buf[0];
			cmd.bus_flag = buf[2] & 0xF0;
			cmd.immediate_flag = (buf[3] & 0xF0) ? 0 : 1;

			cmd.time = (buf[iPos] << 24) | (buf[iPos + 1] << 16) |
				(buf[iPos + 2] << 8) | buf[iPos + 3];
			iPos += 4;

			cmd.cmd_id = buf[iPos];
			pCmdInfo = m_pCmdInfo[cmd.cmd_id];
			if (!pCmdInfo){
				MessageBox("指令码错误,导入注入数据失败！", "错误");
				return;
			}
			iPos++;
			memcpy(cmd.args, buf + iPos, pCmdInfo->arg_byte_num);
			iPos += pCmdInfo->arg_byte_num;
			AddCmdToList(&cmd, -1, 1);
		}
	}
	pldFile.Close();
}
void CDlgCommandSheet::OnBnClickedButtonInCmd()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, "QCmdList", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR,
		"QCmdXml Files (*.dat)|*.dat||", NULL);
	//		"QCmdList Files (*.cls)|*.cls|QCmdLog Files (*.clg)|*.clg|QCmdXml Files (*.xml)|*.xml||", NULL);
	if (dlg.DoModal() != IDOK)
		return;

	m_ListCtrlCommand.DeleteAllItems();
	CString strFileName = dlg.GetPathName();
	CString strExt = dlg.GetFileExt();
	LoadFromPLD(strFileName);
	
}
void CDlgCommandSheet::GetCMDArray(CString fileName)
{
	FILE *fp;
	m_iRealCmdCnt = 0;
	int iCmdCntLastAllInjection = 0, iOffset;
	unsigned char ucInjectionID[2];
	unsigned char ucInjectionIdx;
	unsigned char ucInjectionByteLen;
	unsigned char *pucInjectionContent;
	fp = fopen(fileName, "rb");
	unsigned char fileHeader_cnt_64B[2];
// 	if (!m_bNoTimeCode)
// 	{
// 		fread(fileHeader_cnt_64B, 2, 1, fp);
// 	}
	if (fp != NULL)
	{
		while (!feof(fp))
		{
			fread(ucInjectionID, 2, 1, fp);
			fread(&ucInjectionIdx, 1, 1, fp);
			if (fread(&ucInjectionByteLen, 1, 1, fp) != 1)
				break;
			pucInjectionContent = (unsigned char*)malloc(ucInjectionByteLen);
			fread(pucInjectionContent, ucInjectionByteLen, 1, fp);
			iOffset = 0;
			unsigned char ucFirstByteInjectionContent;
			while (iOffset < (ucInjectionByteLen - 1))
			{
				ucFirstByteInjectionContent = *(pucInjectionContent + iOffset);
				iOffset += 1;
				if (ucFirstByteInjectionContent == 0x5a)	break;
				iCmdCntLastAllInjection = iCmdCntLastAllInjection + (ucFirstByteInjectionContent & 0x0f);
				if ((ucFirstByteInjectionContent >> 4) == 0x09)//每条事件都有时间
				{
					for (; m_iRealCmdCnt < iCmdCntLastAllInjection; m_iRealCmdCnt++)
					{
						//						m_CMDArray[m_iRealCmdCnt].ShipTime = QKDTimer::GetShipTime(pucInjectionContent+iOffset);
						iOffset += 6;
						m_CMDArray[m_iRealCmdCnt].ID = *(pucInjectionContent + iOffset);
						iOffset += 1;
						int iArgByteLength = GetArgsByteLength(m_CMDArray[m_iRealCmdCnt].ID);
						m_CMDArray[m_iRealCmdCnt].ArgByteLength = iArgByteLength;
						memcpy(m_CMDArray[m_iRealCmdCnt].Args, pucInjectionContent + iOffset, iArgByteLength);
						iOffset += iArgByteLength;

						FillNote(&m_CMDArray[m_iRealCmdCnt]);
					}
				}
				else if ((ucFirstByteInjectionContent >> 4) == 0x03)
				{
					for (; m_iRealCmdCnt < iCmdCntLastAllInjection; m_iRealCmdCnt++)
					{
						m_CMDArray[m_iRealCmdCnt].ShipTime = 0;
						m_CMDArray[m_iRealCmdCnt].ID = *(pucInjectionContent + iOffset);
						iOffset += 1;
						int iArgByteLength = GetArgsByteLength(m_CMDArray[m_iRealCmdCnt].ID);
						m_CMDArray[m_iRealCmdCnt].ArgByteLength = iArgByteLength;
						memcpy(m_CMDArray[m_iRealCmdCnt].Args, pucInjectionContent + iOffset, iArgByteLength);
						iOffset += iArgByteLength;

						FillNote(&m_CMDArray[m_iRealCmdCnt]);
					}
				}
				else if ((ucFirstByteInjectionContent >> 4) == 0x06)
				{
				}
			}
			free(pucInjectionContent);
		}
	}
}
int CDlgCommandSheet::GetArgsByteLength(unsigned char ucID)
{
	int iArgsByteLength = 0;

	xmlXPathObjectPtr xpathObj;
	xmlNodePtr pNode;
	xmlChar *xmlRtn;

	char xpath_expr[200];

	sprintf(xpath_expr, "//Command[translate(@code,'abcdef','ABCDEF')='%02X']\0", ucID);

	xpathObj = m_xml.GetNodesByXPath(BAD_CAST(xpath_expr));
	if (xpathObj){
		if (xpathObj->nodesetval->nodeNr == 0){
			xmlXPathFreeObject(xpathObj);
			return 0;
		}
		pNode = xpathObj->nodesetval->nodeTab[0];
		xmlRtn = xmlGetProp(pNode, BAD_CAST("argByteLength"));
		iArgsByteLength = atoi((char *)xmlRtn);
		xmlFree(xmlRtn);
	}
	xmlXPathFreeObject(xpathObj);

	return iArgsByteLength;
}
void CDlgCommandSheet::GetCMDArrayFromLog(CString fileName)
{
	FILE *fp;
	m_iRealCmdCnt = 0;

	__int64 time;
	int type;
	int length;
	unsigned char content[200];

	int byteLength;

	fp = fopen(fileName, "rb");
	while (fp != NULL){
		if (!fread(&time, 8, 1, fp)){
			break;
		};
		if (!fread(&type, 4, 1, fp)){
			break;
		}
		if (!fread(&length, 4, 1, fp)){
			break;
		}
		if (length > 200 || length < 0){
			break;
		}
		if (!fread(content, length, 1, fp)){
			break;
		}
		if (!fread(&length, 4, 1, fp)){
			break;
		}
		if (type != 0x00 && type != 0x01){
			continue;
		}
		if (content[4] != 0x79){
			continue;
		}

		byteLength = GetArgsByteLength(content[5]);
		m_CMDArray[m_iRealCmdCnt].ShipTime = time;
		m_CMDArray[m_iRealCmdCnt].ID = content[5];
		m_CMDArray[m_iRealCmdCnt].ArgByteLength = byteLength;
		memcpy(m_CMDArray[m_iRealCmdCnt].Args, content + 6, byteLength);
		FillNote(&m_CMDArray[m_iRealCmdCnt]);
		m_iRealCmdCnt++;
	}
}

void CDlgCommandSheet::GetCMDArrayFromXml(CString fileName)
{
	CXML xml;

	xml.Open((char *)(LPCTSTR)fileName);

	xmlXPathObjectPtr xpathObj;
	xmlNodePtr pNode;
	xmlChar *xmlRtn;
	int byteLength;
	__int64 time = 0;

	int i, j;

	char xpath_expr[200];

	m_iRealCmdCnt = 0;

	char *endptr;

	sprintf(xpath_expr, "//Command[@code and @argByteLength]\0");
	xpathObj = xml.GetNodesByXPath(BAD_CAST(xpath_expr));
	if (!xpathObj){
		return;
	}

	for (i = 0; i < xpathObj->nodesetval->nodeNr; i++){
		pNode = xpathObj->nodesetval->nodeTab[i];

		xmlRtn = xmlGetProp(pNode, (xmlChar *)"code");
		m_CMDArray[m_iRealCmdCnt].ID = (unsigned char)(strtol((char *)xmlRtn, &endptr, 16) & 0xFF);
		xmlFree(xmlRtn);

		xmlRtn = xmlGetProp(pNode, BAD_CAST("argByteLength"));
		byteLength = atoi((char *)xmlRtn);
		xmlFree(xmlRtn);

		if (byteLength != GetArgsByteLength(m_CMDArray[m_iRealCmdCnt].ID)){
			//			continue;
		}

		m_CMDArray[m_iRealCmdCnt].ArgByteLength = byteLength;

		pNode = pNode->xmlChildrenNode;
		while (pNode != NULL){
			if (!xmlStrcmp(pNode->name, BAD_CAST("Time"))){
				xmlRtn = xmlGetProp(pNode, (xmlChar *)"value");
				if (xmlRtn[0] == '+'){
					time += atof((char *)xmlRtn) * 1000;
				}
				else{
					time = atof((char *)xmlRtn) * 1000;
				}
				if (time < 0){
					time = 0;
				}
				xmlFree(xmlRtn);
			}
			if (!xmlStrcmp(pNode->name, BAD_CAST("Args"))){
				xmlRtn = xmlGetProp(pNode, (xmlChar *)"value");
				endptr = (char *)xmlRtn;
				for (j = 0; j < byteLength; j++){
					m_CMDArray[m_iRealCmdCnt].Args[j] = strtol(endptr, &endptr, 16);
				}
				xmlFree(xmlRtn);
				m_CMDArray[m_iRealCmdCnt].ShipTime = time;
			}
			pNode = pNode->next;
		}
		m_CMDArray[m_iRealCmdCnt].ShipTime = time;
		FillNote(&m_CMDArray[m_iRealCmdCnt]);
		int iAimPosition = FindPosition(true, m_iRealCmdCnt - 1);
		ResortCMD(iAimPosition, m_iRealCmdCnt - 1);
		m_iRealCmdCnt++;
	}
	xmlXPathFreeObject(xpathObj);
}

void CDlgCommandSheet::OnBnClickedButtonOutCmd()
{
	// TODO: Add your control notification handler code here
	FILE *fp;
	int cmdIndex = 0;
	int cmdCntIn64B = 0;
	int cnt_64B = 0;
	int leftByteOf64B = 64;

	char filter[] = "QCmdList Files(*.cls) |*.cls|QCmdTxt Files(*.txt) |*.txt||";
	
	COleDateTime t;
	t = COleDateTime::GetCurrentTime();
	char projectDir[256];
	char cmdfilename[256];
	GetCurrentDirectory(256, projectDir);
	strcpy(cmdfilename, projectDir);
	strcat(cmdfilename, "\\CMD\\");
	if (_access(cmdfilename, 0) != 0)
	{
		_mkdir(cmdfilename);
	}
	CString FileName=cmdfilename;

	CString strVer;
	GetWindowText(strVer);	
	FileName += t.Format("%Y%m%d%H%M%S_");

	FileName += "cmd.dat";

	CFile pld_file;

	pld_file.Open(FileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, NULL);
	SaveToPLD(&pld_file);
	pld_file.Close();
	AfxMessageBox(_T("导出成功！"));

}

void CDlgCommandSheet::displayList(bool isFirst){
	CmdInfo *pCmdInfo;
	m_listCurrentNum = 0;
//	m_listMonitor.DeleteAllItems();
	for (int i = 0; i < m_MonitorCmdNum; i++)
	{
		pCmdInfo = m_pCmdInfo_Recv[i];
		MonitorDisplay(pCmdInfo, m_listCurrentNum, isFirst);
		m_listCurrentNum += pCmdInfo->arg_num;
	}
}

//void CDlgCommandSheet::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: Add your message handler code here and/or call default
////	CmdInfo *pCmdInfo;
//	switch (nIDEvent)
//	{
//	case 2:
//		displayList();
//		break;
//	default:
//		break;
//	}
//
//	CDialogEx::OnTimer(nIDEvent);
//}


void CDlgCommandSheet::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	OnBnClickedButtonOutCmd();
	m_pInterface->SendCmd(m_COMportNum,0, m_CMD_length, (unsigned char *)m_CMDBuf, false);

//	CDialogEx::OnOK();
}


void CDlgCommandSheet::OnBnClickedButtonOpencom()
{
	// TODO: Add your control notification handler code here
	CString strCOMname;
	CString s;
	m_ComboPort.GetWindowTextA(s);
	s.Delete(0, 3);
	m_COMportNum = atoi(s);
	HANDLE hThread_recv=NULL;
	GetDlgItem(IDC_BUTTON_OPENCOM)->GetWindowTextA(strCOMname);
	RECVPARAM *pRecvParam = new RECVPARAM;
	pRecvParam->aa = this;
	if (strCOMname == "打开串口")
	{
		m_ComStatus = m_pInterface->OpenComm(m_COMportNum);
		if (m_ComStatus)
		{
			hThread_recv = CreateThread(NULL, 0, RecvGPSProc, (LPVOID)pRecvParam, 0, NULL);
			GetDlgItem(IDC_BUTTON_OPENCOM)->SetWindowText(_T("关闭串口"));
		}

	}		
	else if (strCOMname == "关闭串口")
	{
		m_displayMonitor = false;
		bool rec = m_pInterface->CloseComm(m_COMportNum);
		if (rec)
		{
			GetDlgItem(IDC_BUTTON_OPENCOM)->SetWindowText(_T("打开串口"));
			TerminateThread(hThread_recv, EXIT_FAILURE);
			CloseHandle(hThread_recv);
			KillTimer(2);
			m_listMonitor.DeleteAllItems();
		}		
	}
	
}

void CDlgCommandSheet::MonitorDisplay(CmdInfo *pCmdInfo, int listCurrentNum, bool isFirst)
{
	int i = 0, k = 0;
	

	CString strBuf, strArg, strBuf1;
	if (!pCmdInfo){
		return;
	}
	int index;
	unsigned char tempbuf[MAX_ARG_LENGTH];
	index = listCurrentNum;
	for (i = 0; i < pCmdInfo->arg_num; i++)
	{
		index = listCurrentNum + i;
		ExtractArgValue(tempbuf, pCmdInfo->init_value, pCmdInfo->bit_start[i], pCmdInfo->arg_length[i]);
		if (isFirst){
			m_listMonitor.InsertItem(index, "");
			strBuf.Format("%0d", index);
			m_listMonitor.SetItemText(index, 0, strBuf);
			m_listMonitor.SetItemText(index, 1, (char *)pCmdInfo->cmd_name);
			
			for (k = 0; k < pCmdInfo->arg_length[i] / 8; k++){
				strBuf1.Format("%02X", tempbuf[k]);
				strBuf += strBuf1;
			}
			if (pCmdInfo->arg_length[i] & 0x7){
				if (pCmdInfo->input_type[i] == 0){
					tempbuf[k] >>= (8 - (pCmdInfo->arg_length[i] & 0x7));
				}
				strBuf1.Format("%02X", tempbuf[k]);
				strBuf += strBuf1;
			}
			strBuf += " ";

			//m_ListCtrlCommand.SetItemText(index, COL_ARG, strBuf);
			//////////////////////////////////////////////////////////////////////
			m_listMonitor.SetItemText(index, 2, (char *)pCmdInfo->arg_name[i]);
		}
		if (pCmdInfo->input_type[i] == 0)//combox
		{				
			for (int j = 0; j < pCmdInfo->combcntNum[i];j++)
			{					
				if (tempbuf[0] == pCmdInfo->Arg_CombxCode[i][j])//init_value 是从高位到地位填充
				{
					m_listMonitor.SetItemText(index, 3, (char *)pCmdInfo->Arg_CombxName[i][j]);
				}
			}
		}
		else                                 //edit
		{			
			long long va = 0;
			int len = (pCmdInfo->arg_length[i] + 7) / 8;
			for (int v = 0; v < len; v++){
				va |= (tempbuf[v] << v * 8);
			}
			if (pCmdInfo->datatype[i] == 0x0a)
			{
				strBuf.Format("%d", va);
			}
			else if (pCmdInfo->datatype[i] == 0xaa){
				strBuf.Format("%lld", va);
			}
			else if (pCmdInfo->datatype[i] == 0x0f)
			{
				strBuf.Format("%08x", va);
			}
			else if (pCmdInfo->datatype[i]==0x0c)
			{
				CString str;
				CString str1(pCmdInfo->cal[i]);
				int pos = str1.Find('x');
				str = str1.Left(pos);
				str.Format(str+"%d", va);
				str += str1.Mid(pos + 1);
				strBuf = Caculate(str);
			}
			m_listMonitor.SetItemText(index, 3, strBuf);
			int in_num, decimal;

		}

	}
}
void CDlgCommandSheet::monitorini(CmdInfo *pCmdInfo[256], CListCtrl listMonitor)
{
	int i = 0, j = 0;
	unsigned char temp[MAX_ARG_LENGTH];
	CString strBuf, strArg;
	int index = 0;
	if (!pCmdInfo){
		return;
	}
	int rowNum = 0;
	for (i = 0; i < monitor_numcnt; i++)
	{
		for (j = 0; j < pCmdInfo[i]->arg_num; j++)
		{
			strBuf.Format("%0d", rowNum);
			m_ListCtrlCommand.SetItemText(rowNum, 0, strBuf);
			strBuf.Format("%s", pCmdInfo[i]->arg_name[j]);
			m_ListCtrlCommand.SetItemText(rowNum, 1, strBuf);
			rowNum++;
		}
	}
}
void CDlgCommandSheet::GetCmdInfo_monitor(CmdInfo *m_pCmdInfo[256])
{
	memset(m_pCmdInfo, 0, sizeof(CmdInfo *)* 256);
	xmlXPathObjectPtr xpathObj;
	char xpath_expr[200];
	int i, j;
	xmlNode *pNode, *pNodeSub;
	xmlChar *xmlRtn;
	char *endptr;
	int idx;
	int length;
	CmdInfo *pCmdInfo;
	CString strTemp;
	unsigned char temp[MAX_ARG_LENGTH];
	unsigned char edit_index, combo_index;
	int sumGB2312;
	int combxCntinArg = 0;
	sprintf(xpath_expr, "/CommandSet/Category/Command");
	xpathObj = LocateXPath(xpath_expr);//查询节点
	monitor_numcnt = xpathObj->nodesetval->nodeNr;
	CString str;
	if (xpathObj){
		m_MonitorCmdNum = xpathObj->nodesetval->nodeNr;
		for (i = 0; i < xpathObj->nodesetval->nodeNr; i++){
			pNode = xpathObj->nodesetval->nodeTab[i];
			xmlRtn = xmlGetProp(pNode, BAD_CAST("CommandCode"));
			idx = strtol((char *)xmlRtn, &endptr, 16);
			idx &= 0xFF;
			xmlFree(xmlRtn);			
			pCmdInfo = (CmdInfo *)malloc(sizeof(CmdInfo));
			m_pCmdInfo[idx] = pCmdInfo;
			memset(pCmdInfo, 0, sizeof(CmdInfo));
			xmlRtn = xmlGetProp(pNode, BAD_CAST("ArgumentByteLength"));
			pCmdInfo->arg_byte_num = strtol((char *)xmlRtn, &endptr, 10);
			xmlFree(xmlRtn);
			xmlRtn = CXML::xmlGetPropGBK(pNode, BAD_CAST("CommandName"));
			length = strlen((char *)xmlRtn);
			if (length > MAX_NAME_LENGTH){
				length = MAX_NAME_LENGTH - 1;
			}
			memcpy(pCmdInfo->cmd_name, xmlRtn, length);
			sumGB2312 = 0;
			for (j = 0; j < length; j++){
				if (pCmdInfo->cmd_name[j] & 0x80){
					sumGB2312++;
				}
			}
			pCmdInfo->cmd_name[length] = '\0';
			if (sumGB2312 & 0x01){
				pCmdInfo->cmd_name[length - 1] = '\0';
			}
			xmlFree(xmlRtn);
			pNode = pNode->xmlChildrenNode;
			idx = 0;
			edit_index = 1;
			combo_index = 0;
			int cnt = 0;
			while (pNode){
				if (idx > MAX_ARG_NUM){
					break;
				}
				cnt++;
				if (!xmlStrcmp(pNode->name, BAD_CAST("Arg"))){//确定节点是否为Arg
					pCmdInfo->input_type[idx] = 1;
					pNodeSub = pNode->xmlChildrenNode;
					combxCntinArg = 0;
					while (pNodeSub){						
						if (!xmlStrcmp(pNodeSub->name, BAD_CAST("ArgValue"))){
							pCmdInfo->input_type[idx] = 0;
							xmlRtn = CXML::xmlGetPropGBK(pNodeSub, BAD_CAST("ArgumentValueName"));
							length = strlen((char *)xmlRtn);
							if (length > MAX_NAME_LENGTH){
								length = MAX_NAME_LENGTH - 1;
							}
							memcpy(pCmdInfo->Arg_CombxName[idx][combxCntinArg], xmlRtn, length);
							sumGB2312 = 0;
							for (j = 0; j < length; j++){
								if (pCmdInfo->Arg_CombxName[idx][combxCntinArg][j] & 0x80){
									sumGB2312++;
								}
							}
							pCmdInfo->Arg_CombxName[idx][combxCntinArg][length] = '\0';
							if (sumGB2312 & 0x01){
								pCmdInfo->Arg_CombxName[idx][combxCntinArg][length - 1] = '\0';
							}
							xmlFree(xmlRtn);							
							xmlRtn = xmlNodeGetContent(pNodeSub);							
							str= xmlRtn;
							
							pCmdInfo->Arg_CombxCode[idx][combxCntinArg] = strtol(str.Left(2),&endptr,16);							
							combxCntinArg++;

						}
						pCmdInfo->combcntNum[idx] = combxCntinArg;
						pNodeSub = pNodeSub->next;
					}
					xmlRtn = xmlGetProp(pNode, BAD_CAST("datatype"));
					if (xmlRtn)
					{
						str = xmlRtn;
						pCmdInfo->datatype[idx] = strtol(str.Left(2), &endptr, 16);
						xmlFree(xmlRtn);
					}					
					
					xmlRtn = CXML::xmlGetPropGBK(pNode, BAD_CAST("cal"));
					if (xmlRtn)
					{
						length = strlen((char *)xmlRtn);
						if (length > MAX_NAME_LENGTH){
							length = MAX_NAME_LENGTH - 1;
						}
						memcpy(pCmdInfo->cal[idx], xmlRtn, length);
					}	
					xmlRtn = xmlGetProp(pNode, BAD_CAST("bitStart"));
					pCmdInfo->bit_start[idx] = atoi((char *)xmlRtn);
					xmlFree(xmlRtn);
					xmlRtn = xmlGetProp(pNode, BAD_CAST("bitLength"));
					pCmdInfo->arg_length[idx] = atoi((char *)xmlRtn);
					xmlFree(xmlRtn);
					xmlRtn = CXML::xmlGetPropGBK(pNode, BAD_CAST("ArgumentName"));
					length = strlen((char *)xmlRtn);
					if (length > MAX_NAME_LENGTH){
						length = MAX_NAME_LENGTH - 1;
					}
					memcpy(pCmdInfo->arg_name[idx], xmlRtn, length);
					sumGB2312 = 0;
					for (j = 0; j < length; j++){
						if (pCmdInfo->arg_name[idx][j] & 0x80){
							sumGB2312++;
						}
					}
					pCmdInfo->arg_name[idx][length] = '\0';
					if (sumGB2312 & 0x01){
						pCmdInfo->arg_name[idx][length - 1] = '\0';
					}
					xmlFree(xmlRtn);
					if (pCmdInfo->bit_start[idx] + pCmdInfo->arg_length[idx] > MAX_ARG_LENGTH * 8){
						break;
					}
//					pCmdInfo->arg_byte_num += pCmdInfo->arg_length[idx];
					xmlRtn = xmlGetProp(pNode, BAD_CAST("initValue"));
					strTemp = xmlRtn;
					xmlFree(xmlRtn);
					strTemp.GetBufferSetLength((pCmdInfo->arg_length[idx] + 7) / 8 * 2);
					for (j = 0; j < pCmdInfo->arg_length[idx] / 8; j++){
						temp[j] = strtol(strTemp.Mid(j * 2, 2), &endptr, 16);
					}
					if (pCmdInfo->arg_length[idx] & 0x7){
						temp[j] = strtol(strTemp.Mid(j * 2, 2), &endptr, 16);
						if (pCmdInfo->input_type[idx] == 0){
							temp[j] <<= (8 - (pCmdInfo->arg_length[idx] & 0x7));
						}
					}
					InsertArgValue(pCmdInfo->init_value, temp, pCmdInfo->bit_start[idx], pCmdInfo->arg_length[idx]);
					if (pCmdInfo->input_type[idx] == 0){
						pCmdInfo->input_ctrl_index[idx] = combo_index;
						combo_index++;
						edit_index += 2;
					}
					else{
						pCmdInfo->input_ctrl_index[idx] = edit_index;
						edit_index += 3;
					}
					idx++;
				}
				pNode = pNode->next;
			}
			pCmdInfo->arg_num = idx;
		}
		xmlXPathFreeObject(xpathObj);
	}
	CXML coef_xml;
	coef_xml.Open("coef.xml");
	for (i = 0; i < 256; i++){
		if (m_pCmdInfo[i]){
			for (j = 0; j < m_pCmdInfo[i]->arg_num; j++){
				m_pCmdInfo[i]->coef[j] = GetCoefficient(&coef_xml, i, m_pCmdInfo[i]->bit_start[j]);
			}
		}
	}
}
DWORD WINAPI CDlgCommandSheet::RecvGPSProc(LPVOID lpParameter)
{
	#define  RecvBuferLength 512
	char RecvBuf[RecvBuferLength] = { 0 };
	char tempBuf[RecvBuferLength] = { 0 };
	int Recvlength = 0, templength = 0;
	CDlgCommandSheet *m_pDlg;
	m_pDlg = ((RECVPARAM*)lpParameter)->aa;
	m_pDlg->m_xml.Open("monitor.xml");
	m_pDlg->GetCmdInfo_monitor(m_pDlg->m_pCmdInfo_Recv);
	m_pDlg->m_xml.Open("commands.xml");
//	m_pDlg->SetTimer(2, 1000, NULL);
	CmdInfo *pCmdInfo;
	int lastBufCntRe = 0;
	int idx;
	m_pDlg->displayList(true);
	while (m_pDlg->m_displayMonitor)
	{
		templength = m_pInterface->RecvCmd(RecvBuferLength, m_COMportNum, tempBuf);
		if (templength > RecvBuferLength){
			m_pInterface->ClearRflush(m_COMportNum);
			Recvlength = 0;
			templength = 0;
			continue;
		}
		memcpy(RecvBuf + Recvlength, tempBuf, templength);
		Recvlength += templength;		
		if (Recvlength >= RecvBuferLength && Recvlength > 0)
		{
			unsigned short crc_Recv = 0,crc_cal=0;
			for (int i = 0; i < RecvBuferLength-2; i++){
				crc_cal += (unsigned char)(RecvBuf[i]);
			}
			crc_Recv = *(unsigned short *)(RecvBuf + RecvBuferLength - 2);
			lastBufCntRe = Recvlength - RecvBuferLength;
			Recvlength = RecvBuferLength;
			if (crc_cal == crc_Recv)
//			if (1)
			{
				int lastCharCnt = 0;
				while (Recvlength > 0)
				{
					idx = RecvBuf[lastCharCnt] & 0xFF;
					if (idx < 0){
						break;
					}
					pCmdInfo = m_pDlg->m_pCmdInfo_Recv[idx];
					if (pCmdInfo == NULL){
						break;
					}
					memcpy(pCmdInfo->init_value, RecvBuf + lastCharCnt + 1, pCmdInfo->arg_byte_num);
					lastCharCnt += pCmdInfo->arg_byte_num + 1;
					Recvlength -= (pCmdInfo->arg_byte_num + 1);
				}
				Recvlength = lastBufCntRe;
				memcpy(RecvBuf, tempBuf + templength - lastBufCntRe, lastBufCntRe);//将多余字符给下一个帧
				templength = 0;
				m_pDlg->displayList(false);
			}
			else
			{
				m_pInterface->ClearRflush(m_COMportNum);
				Recvlength = 0;
				templength = 0;
			}
			
		}
		
		Sleep(500);
	}
	
	return 0;

}

void CDlgCommandSheet::OnBnClickedButtonRead()
{
	// TODO: Add your control notification handler code here
	CString v;
	int i;
	unsigned long addr;
	unsigned long data;
	unsigned char sv[32] = { 0 };
	GetDlgItem(IDC_EDIT_ADDR)->GetWindowTextA(v);
	addr = strtoul(v, NULL, 16);
	memset(sv, 0x5a, 32);
	data = 0;
	i = 0;
	sv[i++] = 0x33;
	sv[i++] = 0x00;
	sv[i++] = 0x00;
	sv[i++] = 0x01;
	sv[i++] = 0x00;
	sv[i++] = 0x00;
	sv[i++] = 0x00;
	sv[i++] = 0x00;
	sv[i++] = 0x88;
	sv[i++] = 0x3c;
	sv[i++] = (addr >> 24) & 0x00ff;
	sv[i++] = (addr >> 16) & 0x00ff;
	sv[i++] = (addr >> 8) & 0x00ff;
	sv[i++] = (addr >> 0) & 0x00ff;
	data = 0;
	for (i = 0; i < 31; i++){
		data +=sv[i];
	}
	sv[31] = data;

	m_pInterface->SendCmd(m_COMportNum, 0, 32, sv, false);
}


void CDlgCommandSheet::OnBnClickedButtonWrite()
{
	// TODO: Add your control notification handler code here
	CString v;
	int i;
	unsigned long addr;
	unsigned long data;
	unsigned char sv[32] = {0};
	GetDlgItem(IDC_EDIT_ADDR)->GetWindowTextA(v);
	addr = strtoul(v, NULL, 16);
	GetDlgItem(IDC_EDIT_DATA)->GetWindowTextA(v);
	data = strtoul(v, NULL, 16);
	memset(sv, 0x5a, 32);
	i = 0;
	sv[i++] = 0x33;
	sv[i++] = 0x00;
	sv[i++] = 0x00;
	sv[i++] = 0x01;
	sv[i++] = 0x00;
	sv[i++] = 0x00;
	sv[i++] = 0x00;
	sv[i++] = 0x00;
	sv[i++] = 0x88;
	sv[i++] = 0xc3;
	sv[i++] = (addr >> 24)&0x00ff;
	sv[i++] = (addr >> 16) & 0x00ff;
	sv[i++] = (addr >> 8) & 0x00ff;
	sv[i++] = (addr >> 0) & 0x00ff;
	sv[i++] = (data >> 24) & 0x00ff;
	sv[i++] = (data >> 16) & 0x00ff;
	sv[i++] = (data >> 8) & 0x00ff;
	sv[i++] = (data >> 0) & 0x00ff;
	data = 0;
	for (i = 0; i < 31; i++){
		data += sv[i];
	}
	sv[31] = data;

	m_pInterface->SendCmd(m_COMportNum, 0, 32, sv, false);
}


void CDlgCommandSheet::OnSize(UINT nType, int cx, int cy)
{
	//CDialogEx::OnSize(nType, cx, cy);

	//CWnd * pWnd;
	//pWnd = GetDlgItem(IDC_LIST);      // 获取控件句柄 
	//if (pWnd) // 判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建 
	//{
	//	CRect rect;    // 获取控件变化前大小 
	//	pWnd->GetWindowRect(&rect);
	//	ScreenToClient(&rect); // 将控件大小转换为在对话框中的区域坐标
	//	// 　cx/m_rect.Width()为对话框在横向的变化比例 
	//	rect.left = rect.left * cx / m_rect.Width(); /**/ /// //调整控件大小 
	//	rect.right = rect.right * cx / m_rect.Width();
	//	rect.top = rect.top * cy / m_rect.Height();
	//	rect.bottom = rect.bottom * cy / m_rect.Height();
	//	pWnd->MoveWindow(rect); // 设置控件大小 
	//}
	// TODO: Add your message handler code here
}

INT getpri(char ptr)
{
	switch (ptr)
	{
	case '+': return 1;
	case '-': return 1;
	case '*': return 2;
	case '/': return 2;
	default: return 0;
	}

}

CString CDlgCommandSheet::Caculate(CString textbuff)
{
	// TODO: 在此添加控件通知处理程序代码
	//获取EDT1输入的字符
	CString decimal;
//	m_edit_input.GetWindowText(textbuff);
	//将字符转成后缀表达式
	operation(textbuff, m_InputNum);

	//根据后缀表达式计算结果
	double u_result = calculation(m_InputNum);

	//在EDT2输出结果
	CString result;
	result.Format("%f", u_result);

	//找到小数点位置
	INT Pointindex = result.Find(".");

	//遍历字符串
	for (INT i = result.GetLength() - 1; i > Pointindex; i--)
	{
		//小数最后位是零
		if (result[i] == '0')
		{
			//如果是小数点的下一位，输出整数
			if (i == Pointindex + 1)
			{
				return result.Left(Pointindex);			
			}
			//继续遍历
			else
				continue;
		}
		//如果小数最后位不是0，输出
		if (result[i] != '0')
		{
			return result.Left(i + 1);
		}

	}
}
void CDlgCommandSheet::operation(CString TBuff, vector<CString> & m_InputNum)
{
	INT i = 0;
	INT length = TBuff.GetLength();
	stack<char> stack1; //存放运算符的栈
	CString	StrNum = "";
	CString Strck;

	while (i <= length)
	{
		//如果遇到数字，寻找下一个，直到遇到操作符，将数字拼接起来
		if ((TBuff[i] - '0') >= 0 && (TBuff[i] - '0') <= 9 || (TBuff[i] == '.'))
		{
			StrNum += TBuff[i];
			i++;
			continue;
		}

		//如果遇到操作符
		if (TBuff[i] == '+' || TBuff[i] == '-' || TBuff[i] == '*' || TBuff[i] == '/')
		{
			//将拼接起来的数字压进vector
			m_InputNum.push_back(StrNum);
			StrNum = "";

			//如果栈空，直接进栈
			if (stack1.empty())
			{
				stack1.push(TBuff[i]);
				i++;
				continue;
			}

			//如果操作符优先级比栈顶操作符优先级高，入栈
			if (getpri(TBuff[i]) > getpri(stack1.top()))
			{
				stack1.push(TBuff[i]);
			}

			//如果操作符优先级比栈顶操作符优先级低
			else
			{
				while (1)
				{
					//栈顶操作符进入vector
					Strck = stack1.top();
					m_InputNum.push_back(Strck);
					stack1.pop();

					//继续判断优先级
					//栈为空，入栈，退出循环
					if (stack1.empty())
					{
						stack1.push(TBuff[i]);
						break;
					}
					//操作符优先级比栈顶操作符优先级高，入栈，退出循环
					else if (getpri(TBuff[i]) > getpri(stack1.top()))
					{
						stack1.push(TBuff[i]);
						break;
					}
				}
			}
		}
		//将最后面的数字压进vector
		else
		{
			m_InputNum.push_back(StrNum);
			StrNum = "";
		}

		i++;
	}
	//将栈中的操作符压进vector
	while (!stack1.empty())
	{
		Strck = stack1.top();
		m_InputNum.push_back(Strck);
		stack1.pop();
	}
	return;
}
double CDlgCommandSheet::calculation(vector<CString> m_InputNum)
{
	INT i = 0;
	INT length = (INT)m_InputNum.size();
	stack<double> temp; //用来存放数字的栈
	double sum = 0;
	double first, second;
	while (i < length)
	{
		//如果是数字，转成double压进数字栈
		if (('+' != m_InputNum[i]) && ('-' != m_InputNum[i]) && ('*' != m_InputNum[i]) && ('/' != m_InputNum[i]))
		{
			temp.push(atof(m_InputNum[i]));
		}
		//如果是运算符
		else if (('+' == m_InputNum[i]) || ('-' == m_InputNum[i]) || ('*' == m_InputNum[i]) || ('/' == m_InputNum[i]))
		{
			int idx = 0;
			if ('+' == m_InputNum[i])
			{
				idx = 0;
			}
			else if ('-' == m_InputNum[i])
			{
				idx = 1;
			}
			else if ('*' == m_InputNum[i])
			{
				idx = 2;
			}
			else if ('/' == m_InputNum[i])
			{
				idx = 3;
			}

			switch (idx)
			{
			case 0:
				first = temp.top();
				temp.pop();
				second = temp.top();
				temp.pop();
				sum = first + second;
				temp.push(sum);
				break;
			case 1:
				first = temp.top();
				temp.pop();
				second = temp.top();
				temp.pop();
				sum = second - first;
				temp.push(sum);
				break;
			case 2:
				first = temp.top();
				temp.pop();
				second = temp.top();
				temp.pop();
				sum = first * second;
				temp.push(sum);
				break;
			case 3:
				first = temp.top();
				temp.pop();
				second = temp.top();
				temp.pop();
				sum = second / first;
				temp.push(sum);
				break;
			}
		}
		i++;
	}
	return temp.top();
}