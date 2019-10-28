// DlgRefreshSheet.cpp : implementation file
//

#include "stdafx.h"
#include "QKDTrans.h"
#include "DlgRefreshSheet.h"
#include "afxdialogex.h"


// CDlgRefreshSheet dialog

IMPLEMENT_DYNAMIC(CDlgRefreshSheet, CDialogEx)

CDlgRefreshSheet::CDlgRefreshSheet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRefreshSheet::IDD, pParent)
{

}

CDlgRefreshSheet::~CDlgRefreshSheet()
{
}

void CDlgRefreshSheet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_KZTELE_VIEW, m_listcontrl_KZTELE);
}
BOOL CDlgRefreshSheet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	LONG lStyle;
	lStyle = GetWindowLong(m_listcontrl_KZTELE.m_hWnd, GWL_STYLE);
	lStyle &= ~LVS_TYPEMASK;
	lStyle |= LVS_REPORT;
	SetWindowLong(m_listcontrl_KZTELE.m_hWnd, GWL_STYLE, lStyle);

// 	DWORD dwStyle = m_listcontrl_KZTELE.GetExtendedStyle(); //获取当前扩展样式
// 	dwStyle &= ~LVS_EX_FULLROWSELECT; //选中某行使整行高亮（report风格时）
// 	dwStyle |= LVS_EX_GRIDLINES; //网格线（report风格时）
// 	dwStyle &= ~LVS_EX_CHECKBOXES; //item前生成checkbox控件
// 	m_listcontrl_KZTELE.SetExtendedStyle(dwStyle);

	DWORD dwStyle = dwStyle = m_listcontrl_KZTELE.GetExtendedStyle(); //获取当前扩展样式
	dwStyle |= LVS_EX_FULLROWSELECT; //选中某行使整行高亮（report风格时）
	dwStyle |= LVS_EX_GRIDLINES; //网格线（report风格时）
	//	dwStyle |= LVS_EX_CHECKBOXES; //item前生成checkbox控件
	m_listcontrl_KZTELE.SetExtendedStyle(dwStyle); //设置扩展风格

	LV_COLUMN lvc[10];
	int i;
	for (i = 0; i < 10; i++)
	{
		lvc[i].mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	}
	lvc[0].iSubItem = 0;
	lvc[0].pszText = "序号";
	lvc[0].cx = 80;

	lvc[1].iSubItem = 0;
	lvc[1].pszText = "参数名称";
	lvc[1].cx = 200;

	lvc[2].iSubItem = 0;
	lvc[2].pszText = "功能描述";
	lvc[2].cx = 200;

	lvc[3].iSubItem = 0;
	lvc[3].pszText = "参数值";
	lvc[3].cx = 200;

	lvc[4].iSubItem = 0;
	lvc[4].pszText = "";
	lvc[4].cx = 80;

	lvc[5].iSubItem = 0;
	lvc[5].pszText = "序号";
	lvc[5].cx = 80;

	lvc[6].iSubItem = 0;
	lvc[6].pszText = "参数名称";
	lvc[6].cx = 200;

	lvc[7].iSubItem = 0;
	lvc[7].pszText = "功能描述";
	lvc[7].cx = 200;

	lvc[8].iSubItem = 0;
	lvc[8].pszText = "参数值";
	lvc[8].cx = 200;
// 
// 	lvc[9].iSubItem = 0;
// 	lvc[9].pszText = "";
// 	lvc[9].cx = 300;

	m_listcontrl_KZTELE.DeleteAllItems();

	for (i = 0; i < 9; i++)
	{
		m_listcontrl_KZTELE.InsertColumn(i, &lvc[i]);
	}
// 	m_listcontrl_KZTELE.InsertItem(0, "");
// 	m_listcontrl_KZTELE.SetItemText(0, 0, "aaaa");
// 	m_listcontrl_KZTELE.InsertItem(1, "");
// 	m_listcontrl_KZTELE.SetItemText(1, 0, "bbbb");
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CDlgRefreshSheet, CDialogEx)
END_MESSAGE_MAP()
INT getprii(char ptr)
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
void CDlgRefreshSheet::operation(CString TBuff, vector<CString> & m_InputNum)
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
			if (getprii(TBuff[i]) > getprii(stack1.top()))
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
					else if (getprii(TBuff[i]) > getprii(stack1.top()))
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
double CDlgRefreshSheet::calculation(vector<CString> m_InputNum)
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
CString CDlgRefreshSheet::Caculate(CString textbuff)
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
void CDlgRefreshSheet::ExtractArgValue(unsigned char *pDst, unsigned char *pSrc, int bitStart, int length)
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
void CDlgRefreshSheet::MonitorDisplay(CmdInfo *pCmdInfo, int listCurrentNum, bool isFirst)
{
	int i = 0, k = 0;

	
	CString strBuf, strArg, strBuf1;
	if (!pCmdInfo){
		return;
	}
	int index;
	unsigned char tempbuf[MAX_ARG_LENGTH];
	index = listCurrentNum;
	int coDex = 0;
	for (i = 0; i < pCmdInfo->arg_num; i++)
	{
		index = listCurrentNum + i;
		if (index>=m_TotalTeleNUM)
		{
			index -= m_TotalTeleNUM;
			coDex = 5;
		}
		ExtractArgValue(tempbuf, pCmdInfo->init_value, pCmdInfo->bit_start[i], pCmdInfo->arg_length[i]);
		if (isFirst){
			if (coDex == 0)
			{
				m_listcontrl_KZTELE.InsertItem(index, "");
				strBuf.Format("%0d", index);
				m_listcontrl_KZTELE.SetItemText(index, 0 + coDex, strBuf);
			}			
			else
			{
				strBuf.Format("%0d", index + m_TotalTeleNUM);
				m_listcontrl_KZTELE.SetItemText(index, 0 + coDex, strBuf);
			}
			m_listcontrl_KZTELE.SetItemText(index, 1 + coDex, (char *)pCmdInfo->cmd_name);

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
			m_listcontrl_KZTELE.SetItemText(index, 2 + coDex, (char *)pCmdInfo->arg_name[i]);
		}
		if (pCmdInfo->input_type[i] == 0)//combox
		{
			for (int j = 0; j < pCmdInfo->combcntNum[i]; j++)
			{
				if (tempbuf[0] == pCmdInfo->Arg_CombxCode[i][j])//init_value 是从高位到地位填充
				{
					m_listcontrl_KZTELE.SetItemText(index, 3 + coDex, (char *)pCmdInfo->Arg_CombxName[i][j]);
				}
			}
		}
		else                                 //edit
		{
			long long va = 0;
			int len = (pCmdInfo->arg_length[i] + 7) / 8;
			switch (len){
			case 1:
				va = *(unsigned char*)(tempbuf);
				break;
			case 2:
				va = *(unsigned short*)(tempbuf);
				break;
			case 4:
				va = *(unsigned int*)(tempbuf);
				break;
			case 8:
				va = *(long long*)(tempbuf);
				break;
			}
			//for (int v = 0; v < len; v++){
			//	va |= (tempbuf[v] << v * 8);
			//}
			if (pCmdInfo->datatype[i] == 0x0a)
			{
				strBuf.Format("%u", va);
			}
			else if (pCmdInfo->datatype[i] == 0xaa){
				strBuf.Format("%lld", va);
			}
			else if (pCmdInfo->datatype[i] == 0x0f)
			{
				strBuf.Format("%08x", va);
			}
			else if (pCmdInfo->datatype[i] == 0x0c)
			{
				CString str;
				CString str1(pCmdInfo->cal[i]);
				int pos = str1.Find('x');
				str = str1.Left(pos);
				str.Format(str + "%d", va);
				str += str1.Mid(pos + 1);
				strBuf = Caculate(str);
			}
			m_listcontrl_KZTELE.SetItemText(index, 3 + coDex, strBuf);
			int in_num, decimal;

		}

	}
}
void CDlgRefreshSheet::displayList(bool isFirst, CmdInfo *pCmdInfo_Recv[256], int MonitorCmdNum)
{
	CmdInfo *pCmdInfo;
	int listCurrentNum = 0;
	//	m_listMonitor.DeleteAllItems();
	m_TotalTeleNUM = 0;
	for (int j = 0; j < MonitorCmdNum; j++)
	{
		pCmdInfo = pCmdInfo_Recv[j];
		m_TotalTeleNUM += pCmdInfo->arg_num;
	}
	m_TotalTeleNUM = ceil(m_TotalTeleNUM/2);
	for (int i = 0; i < MonitorCmdNum; i++)
	{
		pCmdInfo = pCmdInfo_Recv[i];
		MonitorDisplay(pCmdInfo, listCurrentNum, isFirst);
		listCurrentNum += pCmdInfo->arg_num;
	}
}

// CDlgRefreshSheet message handlers
