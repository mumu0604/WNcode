/*
	$Id: QKDDisplay.h 631 2012-07-02 17:34:33Z lwy $
	$URL: http://172.17.8.1/svn/QUESS/tags/QKDBench-2.2.1/QKDDisplay.h $
*/
#ifndef __QKDDISPLAY_H__
#define __QKDDISPLAY_H__

#include "Xml.h"
#include "lua.hpp"
#include <afxcmn.h>



class QKDDisplay{
private:
	CXML *m_pXmlMonitor;
	CXML *m_pXmlMessage;
	CListCtrl *m_pListMonitor;
	CListCtrl *m_pListMessage;
	lua_State *m_Lua;
public:
	QKDDisplay();
	~QKDDisplay();

	void Open(char *fileMonitor, char *fileMessage, CListCtrl *pListMonitor, CListCtrl *pListMessage);
	void Close();
	void FillMessageHead();
	void FillMonitorHead();

	static double QKDDisplay::LuaProcess(lua_State *pLua, unsigned char *pBuf, int startByte, int byteLength, int dispIndex, char *msg);

	void FormDisplayString(unsigned char *pBuf, int startByte, int byteLength, int dispIndex,CString &strDisplay);

	void DisplayMessage(unsigned char *pBuf);
	void DisplayMonitor(unsigned char *pBuf);



};

#endif
