#include "stdafx.h"
#include "Interface.h"
#include "pcomm.h"

CInterface::CInterface()
{
//	OpenComm();
}


CInterface::~CInterface()
{
}

bool CInterface::OpenComm(int PortNum)
{	 
	//设置参数，115200波特率 端口号5
	if (sio_open(PortNum) == 0 && sio_ioctl(PortNum, B115200, BIT_8) == 0)
	{
		
		AfxMessageBox(_T("串口打开成功！"));
		return true;
	}
	else
	{
		AfxMessageBox(_T("串口打开失败！"));
		return false;
	}
}
bool  CInterface::CloseComm(int PortNum)
{
	int rev= sio_close(PortNum);
	if (rev == 0)
	{
		AfxMessageBox(_T("串口关闭成功！"));
		return true;
	}
	else
	{
		AfxMessageBox(_T("串口关闭失败！"));
		return false;
	}
}
int CInterface::RecvCmd(int RecvLength, int PortNum, char RecvBuf[100])
{
	RecvLength = sio_read(PortNum, (char *)RecvBuf, 32);
	return RecvLength;
}
void CInterface::ClearRflush(int PortNum)
{
	sio_flush(PortNum, 1);
}
void CInterface::SendCmd(int PortNum, int type, int byteLen, unsigned char *pContent, bool isLocal)
{
	unsigned char buf[70];
	 sio_flush(PortNum, 0);
	 int len11 = sio_write(PortNum, (char *)pContent, byteLen);
	 if (len11 == -1)
	 {
		 AfxMessageBox(_T("指令发送失败，请检查！"));
	 }


		
// 		if (sio_open(5) == 0)
// 		{			
// 			int	len = sio_read(5, (char *)buf, 1);//发送数据
// 			sio_write(5, (char *)buf, frameLen);//接收数据 5是本端端口号
// 			sio_SetReadTimeouts(5, 0, 0);//设置串口写操作的超时；
// 			sio_flush(5, 2);//清空缓存区 0:flush input buffer  1:flush output buffer 		2 : flush input & output buffer  *
// 			sio_lctrl(PortNum, 0);// 设置串口RTS / DTS。
// 			sio_read(PortNum, (char *)buf, len);// 从输入缓冲区读指定个数的字符
// 			sio_getch(PortNum); //从输入缓冲区读一个字符
// 			sio_iqueue(PortNum);// 读取输入缓冲区中的字符长度；
// 			sio_write(PortNum, (char *)buf, len); //写指定个数的字符到输出缓冲区；
// 			sio_close(5);//关闭端口
// 			sio_AbortRead(5);
// 		}

}
int CInterface::FormatCmdFrame(int type, int byteLen, unsigned char *pContent, unsigned char *pCmdFrame)
{
	int cmdFrameLength = 0;

	*pCmdFrame = 0xA5;
	*(pCmdFrame + 1) = 0xA5;
	*(pCmdFrame + 2) = 0xC3;

	if (byteLen <= 6){
		*(pCmdFrame + 3) = 0x03;
		cmdFrameLength = 6;
	}
	else if (byteLen <= 10){
		*(pCmdFrame + 3) = 0x0C;
		cmdFrameLength = 10;
	}
	else if (byteLen <= 22){
		*(pCmdFrame + 3) = 0x30;
		cmdFrameLength = 22;
	}
	else if (byteLen <= 33){
		*(pCmdFrame + 3) = 0xC0;
		cmdFrameLength = 33;
	}
	else if (byteLen <= 64){
		*(pCmdFrame + 3) = 0x33;
		cmdFrameLength = 64;
	}
	else{
		return 0;
	}
// 	switch (type)
// 	{
// 	case cmdType_indirect:
// 		*(pCmdFrame + 4) = 0x79;//间接指令
// 		break;
// 	case cmdType_Plat:
// 		*(pCmdFrame + 4) = 0x7C;
// 		break;
// 	case cmdType_GPS:
// 		*(pCmdFrame + 4) = 0x76;
// 		break;
// 	case cmdType_shipTime:
// 		*(pCmdFrame + 4) = 0x7F;
// 		break;
// 	default:
// 		return 0;
// 	}

	memcpy(pCmdFrame + 5, pContent, byteLen);
	memset(pCmdFrame + 5 + byteLen, 0x47, cmdFrameLength - byteLen);
	*(pCmdFrame + 5 + cmdFrameLength) = 0x00;
	for (int i = 2; i < 5 + cmdFrameLength; i++)
	{
		*(pCmdFrame + 5 + cmdFrameLength) ^= *(pCmdFrame + i);
	}
	return 6 + cmdFrameLength;
}