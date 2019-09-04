#pragma once
class CInterface
{
public:
	CInterface();
	~CInterface();
	int CInterface::RecvCmd(int RecvLength, int PortNum, char* RecvBuf);
	bool SendCmd(int PortNum, int type, int byteLen, unsigned char *pContent, bool isLocal);
	int FormatCmdFrame(int type, int byteLen, unsigned char *pContent, unsigned char *pCmdFrame);
	bool OpenComm(int PortNum);
	bool  CloseComm(int PortNum);
	void ClearRflush(int PortNum);
};

