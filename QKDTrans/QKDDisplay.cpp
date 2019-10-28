/*
	$Id: QKDDisplay.cpp 631 2012-07-02 17:34:33Z lwy $
	$URL: http://172.17.8.1/svn/QUESS/tags/QKDBench-2.2.1/QKDDisplay.cpp $
*/
#include "stdafx.h"
#include <afx.h>
#include <math.h>
#include "QKDDisplay.h"
#include "QKDTimer.h"

double QKDDisplay::LuaProcess(lua_State *pLua, unsigned char *pBuf, int startByte, int byteLength, int dispIndex, char *msg)
{
	
	msg[0] = 0;
	if(!pLua){
		return 0;
	}
	
	char fname[200];
	
	sprintf(fname,"Calc%02d\0",dispIndex);
	lua_getglobal(pLua,fname);
	
	lua_pushlstring(pLua,(char *)pBuf+startByte, byteLength);
	
	lua_pcall(pLua,1,2,0);
	
	LUA_NUMBER f;
	
	f = lua_tonumber(pLua,-2); 
	
	const char *pMsg;
	
	pMsg = lua_tostring(pLua,-1);
	
	if(pMsg){
		memcpy(msg, pMsg,strlen(pMsg) + 1);
	}
	
	lua_pop(pLua,2);
	
	return f;
}


QKDDisplay::QKDDisplay()
{
	m_pListMessage = NULL;
	m_pListMonitor = NULL;
	m_pXmlMonitor = NULL;
	m_pXmlMessage = NULL;
 	m_Lua = luaL_newstate();
	if(luaL_loadfile(m_Lua,"display.lua")){
		lua_close(m_Lua);
		m_Lua = NULL;
	}
	if(lua_pcall(m_Lua,0,0,0)){
		lua_close(m_Lua);
		m_Lua = NULL;
	}
	if(m_Lua){
		luaL_openlibs(m_Lua);
	}
}

QKDDisplay::~QKDDisplay()
{
	Close();
	if(m_Lua){
		lua_close(m_Lua);
	}
}

void QKDDisplay::Open(char *fileMonitor, char *fileMessage, CListCtrl *pListMonitor, CListCtrl *pListMessage)
{
	HWND hWnd;
	hWnd = pListMonitor->GetSafeHwnd();
	if(IsWindow(hWnd)){
		m_pXmlMonitor = new CXML;
		m_pXmlMonitor->Open(fileMonitor);		
		m_pListMonitor = pListMonitor;
		FillMonitorHead();
	}
	else{
		m_pListMonitor = NULL;
	}

	hWnd = pListMessage->GetSafeHwnd();
	if(IsWindow(hWnd)){
		m_pXmlMessage = new CXML;
		m_pXmlMessage->Open(fileMessage);
		m_pListMessage = pListMessage;
		FillMessageHead();

	}
	else{
		m_pListMessage = NULL;
	}
}

void QKDDisplay::Close()
{
	if(m_pXmlMonitor){
		delete m_pXmlMonitor;
	}
	if(m_pXmlMessage){
		delete m_pXmlMessage;
	}
}

void QKDDisplay::FillMonitorHead()
{
	if(!m_pListMonitor){
		return;
	}

	LONG lStyle;
	lStyle = GetWindowLong(m_pListMonitor->m_hWnd, GWL_STYLE);
	lStyle &= ~LVS_TYPEMASK;
	lStyle |= LVS_REPORT;
	SetWindowLong(m_pListMonitor->m_hWnd, GWL_STYLE, lStyle);
	
	DWORD dwStyle = dwStyle = m_pListMonitor->GetExtendedStyle(); //获取当前扩展样式
	dwStyle |= LVS_EX_FULLROWSELECT; //选中某行使整行高亮（report风格时）
	dwStyle |= LVS_EX_GRIDLINES; //网格线（report风格时）
	//	dwStyle |= LVS_EX_CHECKBOXES; //item前生成checkbox控件
	m_pListMonitor->SetExtendedStyle(dwStyle); //设置扩展风格
	
	m_pListMonitor->SetExtendedStyle(dwStyle);
	


	LV_COLUMN lvc[5];
	int i;
	for (i = 0; i < 5; i++)
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
	lvc[4].cx = 300;

	
	for (i = 0; i < 5; i++)
	{
		m_pListMonitor->InsertColumn(i, &lvc[i]);
	}
}


void QKDDisplay::FillMessageHead()
{
	if(!m_pListMessage){
		return;
	}

	CListCtrl* pListCtrl = m_pListMessage;
	int item = 0;
	int rowIndex;
	
	xmlXPathObjectPtr xpathObj;
	xmlNodePtr pNode,pChild;
	xmlChar *xmlRtn;
	int i;
	int blockByte,startByte;
	
	CString idsName, strByteLength, strByteOffset, strBitOffset;
	

	LONG lStyle;
	lStyle = GetWindowLong(pListCtrl->m_hWnd, GWL_STYLE);
	lStyle &= ~LVS_TYPEMASK;
	lStyle |= LVS_REPORT;
	SetWindowLong(pListCtrl->m_hWnd, GWL_STYLE, lStyle);
	
	DWORD dwStyle = pListCtrl->GetExtendedStyle(); 
	dwStyle |= LVS_EX_FULLROWSELECT; 
	dwStyle |= LVS_EX_GRIDLINES; 
	pListCtrl->SetExtendedStyle(dwStyle); 
	
	LV_COLUMN lvc[5];
	for (i = 0; i < 5; i++)
	{
		lvc[i].mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	}
	lvc[0].iSubItem = 0;
	lvc[0].pszText = "字节";
	lvc[0].cx = 40;
	
	lvc[1].iSubItem = 0;
	lvc[1].pszText = "参数名称";
	lvc[1].cx = 200;
	
	lvc[2].iSubItem = 0;
	lvc[2].pszText = "长度";
	lvc[2].cx = 40;
	
	lvc[3].iSubItem = 0;
	lvc[3].pszText = "参数值";
	lvc[3].cx = 170;
	
	lvc[4].iSubItem = 0;
	lvc[4].pszText = "参数解析";
	lvc[4].cx = 300;
	
	pListCtrl->DeleteAllItems();

	for (i = 0; i < 5; i++)
	{
		pListCtrl->InsertColumn(i, &lvc[i]);
	}

	xpathObj = m_pXmlMessage->GetNodesByXPath(BAD_CAST("//MessageBlock[@startByte]"));
	
	
	if(xpathObj){
		for(i = 0; i < xpathObj->nodesetval->nodeNr; i++){
			pNode = xpathObj->nodesetval->nodeTab[i];
			xmlRtn = xmlGetProp(pNode,BAD_CAST("startByte"));
			blockByte = atoi((char *)xmlRtn);
			xmlFree(xmlRtn);
			
			pNode = pNode->xmlChildrenNode;
			while(pNode != NULL){
				if(!xmlStrcmp(pNode->name,BAD_CAST("Message"))){
					xmlRtn = CXML::xmlGetPropGBK(pNode,BAD_CAST("name"));
					idsName.Format("%s",xmlRtn);
					xmlFree(xmlRtn);
					xmlRtn = xmlGetProp(pNode,BAD_CAST("byteLength"));
					strByteLength.Format("%s",xmlRtn);
					xmlFree(xmlRtn);
					xmlRtn = xmlGetProp(pNode,BAD_CAST("startByte"));
					startByte = atoi((char *)xmlRtn);
					xmlFree(xmlRtn);
					xmlRtn = CXML::xmlGetPropGBK(pNode,BAD_CAST("name"));
					idsName.Format("%s",xmlRtn);
					xmlFree(xmlRtn);
					
					rowIndex = pListCtrl->InsertItem(item, "");
					strByteOffset.Format("%d", blockByte+startByte);
					pListCtrl->SetItemText(rowIndex, 0, strByteOffset);
					pListCtrl->SetItemText(rowIndex, 1, idsName);
					pListCtrl->SetItemText(rowIndex, 2, strByteLength);
					item++;
					
					xmlRtn = xmlGetProp(pNode,BAD_CAST("parseType"));
					if(!xmlStrcmp(xmlRtn,BAD_CAST("bits"))){
						xmlFree(xmlRtn);
						pChild = pNode->xmlChildrenNode;
						while(pChild != NULL){
							if(!xmlStrcmp(pChild->name,BAD_CAST("Bits"))){
								xmlRtn = CXML::xmlGetPropGBK(pChild,BAD_CAST("name"));
								idsName.Format("%s",xmlRtn);
								xmlFree(xmlRtn);
								xmlRtn = xmlGetProp(pChild,BAD_CAST("startBit"));
								strBitOffset = strByteOffset;
								strBitOffset += ".";
								strBitOffset += (CString)xmlRtn;
								xmlFree(xmlRtn);
								rowIndex = pListCtrl->InsertItem(item, "");
								pListCtrl->SetItemText(rowIndex, 0, strBitOffset);
								pListCtrl->SetItemText(rowIndex, 1, "--"+idsName);
								item++;
							}
							pChild = pChild->next;
						}
					}
					else{
						xmlFree(xmlRtn);
					}
				}
				pNode=pNode->next;
			}
		}
		xmlXPathFreeObject(xpathObj);
	}
}

void QKDDisplay::FormDisplayString(unsigned char *pBuf, int startByte, int byteLength, int dispIndex,CString &strDisplay)
{

	char msg[200];
	LuaProcess(m_Lua,pBuf,startByte,byteLength,dispIndex,msg);
	strDisplay = msg;
	return;
}

void QKDDisplay::DisplayMessage(unsigned char *pBuf)
{
	if(!m_pListMessage){
		return;
	}

	CListCtrl* pListCtrl = m_pListMessage;
	
	int rowIndex = 0;
	
	xmlXPathObjectPtr xpathObj;
	xmlNodePtr pNode,pChild,pBitValue;
	xmlChar *xmlRtn;
	int i,j;
	int blockByte,startByte,byteLength,dispIndex,startBit,bitLength,bitValue;
	CString strData,strRst;
	CString temp;
	
	char *endptr;
	
	xpathObj = m_pXmlMessage->GetNodesByXPath(BAD_CAST("//MessageBlock[@startByte]"));
	
	
	if(xpathObj){
		for(i = 0; i < xpathObj->nodesetval->nodeNr; i++){
			pNode = xpathObj->nodesetval->nodeTab[i];
			xmlRtn = xmlGetProp(pNode,BAD_CAST("startByte"));
			blockByte = atoi((char *)xmlRtn);
			xmlFree(xmlRtn);
			
			pNode = pNode->xmlChildrenNode;
			while(pNode != NULL){
				if(!xmlStrcmp(pNode->name,BAD_CAST("Message"))){
					xmlRtn = xmlGetProp(pNode,BAD_CAST("byteLength"));
					byteLength = atoi((char *)xmlRtn);
					xmlFree(xmlRtn);
					xmlRtn = xmlGetProp(pNode,BAD_CAST("startByte"));
					startByte = atoi((char *)xmlRtn);
					xmlFree(xmlRtn);
					xmlRtn = xmlGetProp(pNode,BAD_CAST("dispIndex"));
					dispIndex = atoi((char *)xmlRtn);
					xmlFree(xmlRtn);
					strData.Format("");
					for(j = 0; j < byteLength; j++){
						temp.Format("%02X",pBuf[j+blockByte+startByte]);
						strData += temp;
					}
					
					pListCtrl->SetItemText(rowIndex, 3, strData);
					
					FormDisplayString(pBuf,startByte+blockByte,byteLength,dispIndex,strRst);
					pListCtrl->SetItemText(rowIndex, 4, strRst);
					rowIndex++;
					
					xmlRtn = xmlGetProp(pNode,BAD_CAST("parseType"));
					if(!xmlStrcmp(xmlRtn,BAD_CAST("bits"))){
						xmlFree(xmlRtn);
						pChild = pNode->xmlChildrenNode;
						while(pChild != NULL){
							if(!xmlStrcmp(pChild->name,BAD_CAST("Bits"))){
								xmlRtn = xmlGetProp(pChild,BAD_CAST("startBit"));
								startBit = atoi((char *)xmlRtn);
								xmlFree(xmlRtn);
								xmlRtn = xmlGetProp(pChild,BAD_CAST("bitLength"));
								bitLength = atoi((char *)xmlRtn);
								xmlFree(xmlRtn);
								strData.Format("");
								for(j = bitLength - 1; j >=0 ; j--){
									temp.Format("%d",(pBuf[startByte+blockByte] & (1 << (startBit+j))) >> (startBit+j));
									strData += temp;
								}
								pListCtrl->SetItemText(rowIndex, 3, strData);
								bitValue = strtol((LPCSTR)strData,&endptr,2);
								strRst = "非法值";
								pBitValue = pChild->xmlChildrenNode;
								while(pBitValue != NULL){
									if(!xmlStrcmp(pBitValue->name,BAD_CAST("BitsValue"))){
										xmlRtn = xmlGetProp(pBitValue,BAD_CAST("value"));
										if(atoi((char *)xmlRtn) == bitValue){
											xmlFree(xmlRtn);
											xmlRtn = CXML::xmlGetPropGBK(pBitValue,BAD_CAST("name"));
											strRst = xmlRtn;
											xmlFree(xmlRtn);
											break;
										}
										xmlFree(xmlRtn);
									}
									pBitValue = pBitValue->next;
								}
								pListCtrl->SetItemText(rowIndex, 4, strRst);
								rowIndex++;
							}
							pChild = pChild->next;
						}
					}
					else{
						xmlFree(xmlRtn);
					}
				}
				pNode=pNode->next;
			}
		}
		xmlXPathFreeObject(xpathObj);
	}
}

void QKDDisplay::DisplayMonitor(unsigned char *pBuf)
{
	if(!m_pListMonitor){
		return;
	}
	CListCtrl* pListCtrl = m_pListMonitor;
	
	int idx = 0;
	
	xmlXPathObjectPtr xpathObj;
	xmlNodePtr pNode,pChild,pBitValue;
	xmlChar *xmlRtn;
	int i,j;
	int startByte,byteLength,dispIndex,startBit,bitLength,bitValue;
	CString strName,strValue;
	
	
	xpathObj = m_pXmlMonitor->GetNodesByXPath(BAD_CAST("//Message[@startByte]"));
	
	
	if(xpathObj){
		for(i = 0; i < xpathObj->nodesetval->nodeNr; i++){
			pNode = xpathObj->nodesetval->nodeTab[i];
			xmlRtn = xmlGetProp(pNode,BAD_CAST("startByte"));
			startByte = atoi((char *)xmlRtn);
			xmlFree(xmlRtn);
			xmlRtn = xmlGetProp(pNode,BAD_CAST("byteLength"));
			byteLength = atoi((char *)xmlRtn);
			xmlFree(xmlRtn);
			xmlRtn = xmlGetProp(pNode,BAD_CAST("dispIndex"));
			dispIndex = atoi((char *)xmlRtn);
			xmlFree(xmlRtn);
			
			xmlRtn = xmlGetProp(pNode,BAD_CAST("parseType"));
			if(!xmlStrcmp(xmlRtn,BAD_CAST("bits"))){
				xmlFree(xmlRtn);
				pChild = pNode->xmlChildrenNode;
				while(pChild != NULL){
					if(!xmlStrcmp(pChild->name,BAD_CAST("Bits"))){
						xmlRtn = xmlGetProp(pChild,BAD_CAST("startBit"));
						startBit = atoi((char *)xmlRtn);
						xmlFree(xmlRtn);
						xmlRtn = xmlGetProp(pChild,BAD_CAST("bitLength"));
						bitLength = atoi((char *)xmlRtn);
						xmlFree(xmlRtn);
						bitValue = 0;
						for(j = bitLength - 1; j >=0 ; j--){
							bitValue <<= 1;
							bitValue += (pBuf[startByte] & (1 << (startBit+j))) >> (startBit+j);
						}
						xmlRtn = CXML::xmlGetPropGBK(pChild,BAD_CAST("name"));
						strValue.Format("%s%s",xmlRtn,"数据无效");
						xmlFree(xmlRtn);
						pBitValue = pChild->xmlChildrenNode;
						while(pBitValue != NULL){
							if(!xmlStrcmp(pBitValue->name,BAD_CAST("BitsValue"))){
								xmlRtn = xmlGetProp(pBitValue,BAD_CAST("value"));
								if(atoi((char *)xmlRtn) == bitValue){
									xmlFree(xmlRtn);
									xmlRtn = CXML::xmlGetPropGBK(pBitValue,BAD_CAST("name"));
									strValue = xmlRtn;
									xmlFree(xmlRtn);
									break;
								}
								xmlFree(xmlRtn);
							}
							pBitValue = pBitValue->next;
						}
						pListCtrl->SetItemText(idx/5, idx % 5, strValue);
						idx ++;
					}
					pChild = pChild->next;
				}
			}
			else{
				xmlFree(xmlRtn);
				xmlRtn = CXML::xmlGetPropGBK(pNode,BAD_CAST("name"));
				strName = xmlRtn;
				xmlFree(xmlRtn);
				FormDisplayString(pBuf,startByte,byteLength,dispIndex,strValue);
				pListCtrl->SetItemText(idx/5, idx % 5, strName+":"+strValue);
				idx ++;
			}
		}
		xmlXPathFreeObject(xpathObj);
	}
	return;
}

