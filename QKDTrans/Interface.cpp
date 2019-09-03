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
	//���ò�����115200������ �˿ں�5
	if (sio_open(PortNum) == 0 && sio_ioctl(PortNum, B115200, BIT_8) == 0)
	{
		
		AfxMessageBox(_T("���ڴ򿪳ɹ���"));
		return true;
	}
	else
	{
		AfxMessageBox(_T("���ڴ�ʧ�ܣ�"));
		return false;
	}
}
bool  CInterface::CloseComm(int PortNum)
{
	int rev= sio_close(PortNum);
	if (rev == 0)
	{
		AfxMessageBox(_T("���ڹرճɹ���"));
		return true;
	}
	else
	{
		AfxMessageBox(_T("���ڹر�ʧ�ܣ�"));
		return false;
	}
}
int CInterface::RecvCmd(int RecvLength, int PortNum, char* RecvBuf)
{
		return sio_read(PortNum, (char *)RecvBuf, RecvLength);
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
		 AfxMessageBox(_T("ָ���ʧ�ܣ����飡"));
	 }


		
// 		if (sio_open(5) == 0)
// 		{			
// 			int	len = sio_read(5, (char *)buf, 1);//��������
// 			sio_write(5, (char *)buf, frameLen);//�������� 5�Ǳ��˶˿ں�
// 			sio_SetReadTimeouts(5, 0, 0);//���ô���д�����ĳ�ʱ��
// 			sio_flush(5, 2);//��ջ����� 0:flush input buffer  1:flush output buffer 		2 : flush input & output buffer  *
// 			sio_lctrl(PortNum, 0);// ���ô���RTS / DTS��
// 			sio_read(PortNum, (char *)buf, len);// �����뻺������ָ���������ַ�
// 			sio_getch(PortNum); //�����뻺������һ���ַ�
// 			sio_iqueue(PortNum);// ��ȡ���뻺�����е��ַ����ȣ�
// 			sio_write(PortNum, (char *)buf, len); //дָ���������ַ��������������
// 			sio_close(5);//�رն˿�
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
// 		*(pCmdFrame + 4) = 0x79;//���ָ��
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