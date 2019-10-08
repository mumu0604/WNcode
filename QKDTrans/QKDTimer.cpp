/*
	$Id: QKDTimer.cpp 624 2012-06-29 04:22:44Z lwy $
	$URL: http://172.17.8.1/svn/QUESS/tags/QKDBench-2.2.1/QKDTimer.cpp $
*/
#include "stdafx.h"
#include "QKDTimer.h"
#include <afx.h>

QKDTimer::QKDTimer()
{
	ClearShipTime();

	m_GPSMode = 0;
	SYSTEMTIME t;
	FILETIME f;
	ULARGE_INTEGER l;

	t.wYear = 1999;
	t.wMonth = 8;
	t.wDay = 22;
	t.wHour = 8;
	t.wMinute = 0;
	t.wSecond = 0;
	t.wMilliseconds = 0;

	SystemTimeToFileTime(&t,&f);
	memcpy(&l,&f,sizeof(FILETIME));
	m_GPSTimeBase = l.QuadPart/10000;
}

QKDTimer::~QKDTimer()
{

}

void QKDTimer::ClearShipTime()
{
	SYSTEMTIME t;
	FILETIME f;
	ULARGE_INTEGER l;

	GetLocalTime(&t);
	SystemTimeToFileTime(&t,&f);
	memcpy(&l,&f,sizeof(FILETIME));

	m_ShipTimeBase = l.QuadPart/10000;
}

__int64 QKDTimer::GetShipTime()
{
	__int64 tm_ms;
	SYSTEMTIME t;
	FILETIME f;
	ULARGE_INTEGER l;

	GetLocalTime(&t);
	SystemTimeToFileTime(&t,&f);
	memcpy(&l,&f,sizeof(FILETIME));
	tm_ms = l.QuadPart/10000;

	return tm_ms - m_ShipTimeBase;
}

__int64 QKDTimer::GetShipTimeS()
{
	__int64 tm_ms;
	SYSTEMTIME t;
	FILETIME f;
	ULARGE_INTEGER l;

	GetLocalTime(&t);
	SystemTimeToFileTime(&t, &f);
	memcpy(&l, &f, sizeof(FILETIME));
	tm_ms = l.QuadPart / 10000;

	return tm_ms;
}

void QKDTimer::SetShipTime(__int64 time)
{
	if(time > GetShipTime()){
		m_ShipTimeBase = m_ShipTimeBase + GetShipTime() - time; 
	}
}

__int64 QKDTimer::GetShipTime(int day, int hour, int min, int sec, int ms)
{
	__int64 tm_ms;
	tm_ms = (__int64)day * 86400000I64 + (__int64)hour * 3600000I64 + (__int64)min * 60000I64 + (__int64)sec * 1000I64 + (__int64)ms;
	return tm_ms;
}

__int64 QKDTimer::GetShipTime(unsigned char *pBuf)
{
	__int64 tm_ms = 0;
	tm_ms = ((__int64)pBuf[0] << 24 | (__int64)pBuf[1] << 16 | (__int64)pBuf[2] << 8 | (__int64)pBuf[3]) * 1000I64;
	tm_ms += ((__int64)pBuf[4] << 8 | (__int64)pBuf[5]) >> 1;
	return tm_ms;
}

void QKDTimer::GetShipTimeCode(__int64 time, unsigned char *pBuf)
{
	int sec,ms;
	sec = GetLargeSecondofShipTime(time);
	ms = GetMillisecondofShipTime(time);
	pBuf[0] = (unsigned char)((sec >> 24) & 0xFF);
	pBuf[1] = (unsigned char)((sec >> 16) & 0xFF);
	pBuf[2] = (unsigned char)((sec >> 8) & 0xFF);
	pBuf[3] = (unsigned char)(sec & 0xFF);
	pBuf[4] = (unsigned char)((ms >> 8) & 0xFF);
	pBuf[5] = (unsigned char)(ms & 0xFF);
}

int QKDTimer::GetDayofShipTime(__int64 ship_time)
{
	return (int)(ship_time/86400000I64);
}

int QKDTimer::GetHourofShipTime(__int64 ship_time)
{
	__int64 tm;
	
	tm = ship_time % 86400000I64;
	
	return (int)(tm/3600000I64);
}

int QKDTimer::GetMinuteofShipTime(__int64 ship_time)
{
	__int64 tm;
	
	tm = ship_time % 3600000I64;
	
	return (int)(tm/60000I64);
}

int QKDTimer::GetSecondofShipTime(__int64 ship_time)
{
	__int64 tm;
	
	tm = ship_time % 60000I64;
	
	return (int)(tm/1000I64);
}

int QKDTimer::GetLargeSecondofShipTime(__int64 ship_time)
{
	return (int)(ship_time/1000I64);
}

int QKDTimer::GetMillisecondofShipTime(__int64 ship_time)
{
	__int64 tm;
	
	tm = ship_time % 1000I64;
	
	return (int)tm;
}

__int64 QKDTimer::GetGPSTimeWithoutVerify()
{
	__int64 tm_ms;
	SYSTEMTIME t;
	FILETIME f;
	ULARGE_INTEGER l;
	
	GetLocalTime(&t);
	SystemTimeToFileTime(&t,&f);
	memcpy(&l,&f,sizeof(FILETIME));
	tm_ms = l.QuadPart/10000;
	
	switch(m_GPSMode)
	{
	case 0:
		return (tm_ms - m_GPSTimeBase)/1000;
	case 1:
		return (tm_ms - m_ShipTimeBase)/1000;
	default:
		return 0;
	}
}
__int64 QKDTimer::GetGPSTimeNowday()
{
	__int64 tm_ms;
	__int64 tm_ms1;

	SYSTEMTIME t;
	SYSTEMTIME t1;
	FILETIME f;
	FILETIME f1;
	ULARGE_INTEGER l;
	ULARGE_INTEGER l1;

	GetLocalTime(&t);
	t1.wDay = t.wDay;
	t1.wDayOfWeek = t.wDayOfWeek;
	t1.wHour = 0;
	t1.wMilliseconds = 0;
	t1.wMinute = 0;
	t1.wMonth = t.wMonth;
	t1.wSecond = 0;
	t1.wYear = t.wYear;
	SystemTimeToFileTime(&t, &f);
	memcpy(&l, &f, sizeof(FILETIME));
	tm_ms = l.QuadPart / 10000;
	SystemTimeToFileTime(&t1, &f1);
	memcpy(&l1, &f1, sizeof(FILETIME));
	tm_ms1 = l1.QuadPart / 10000;

	/////////////////////////////
	return tm_ms1;
	//////////////////////////////

}
__int64 QKDTimer::GetGPSTime()
{
	__int64 tm_ms;
	SYSTEMTIME t;
	FILETIME f;
	ULARGE_INTEGER l;
	
	GetLocalTime(&t);
	SystemTimeToFileTime(&t,&f);
	memcpy(&l,&f,sizeof(FILETIME));
	tm_ms = l.QuadPart/10000;
	return tm_ms;
}

__int64 QKDTimer::GetGPSTime(int week, int sec)
{
	__int64 tm;
	
	if(week >= 1024 || sec >= 604800){
		return 0;
	}
	
	tm = week * 604800I64;
	tm += sec;
	
	return tm;
}

__int64 QKDTimer::GetGPSTime(unsigned char *pBuf)
{
	int week;
	int sec;

	week = (int)pBuf[0] << 8 | (int)pBuf[1];
	sec = (int)pBuf[2] << 24 | (int)pBuf[3] << 16 | (int)pBuf[4] << 8 | (int)pBuf[5];

	return week * 604800I64 + sec;
}

void QKDTimer::GetGPSTimeCode(__int64 time, unsigned char *pBuf)
{
	int week,sec;
	week = GetWeekofGPSTime(time);
	sec = GetSecondofGPSTime(time);

	pBuf[0] = (unsigned char)((week >> 8) & 0xFF);
	pBuf[1] = (unsigned char)(week & 0xFF);
	pBuf[2] = (unsigned char)((sec >> 24) & 0xFF);
	pBuf[3] = (unsigned char)((sec >> 16) & 0xFF);
	pBuf[4] = (unsigned char)((sec >> 8) & 0xFF);
	pBuf[5] = (unsigned char)(sec & 0xFF);
}

int QKDTimer::GetSecondofGPSTime(__int64 gps_time)
{
	__int64 tm;
	
	tm = gps_time % 604800I64;
	
	return (int)(tm);
}


int QKDTimer::GetWeekofGPSTime(__int64 gps_time)
{
	__int64 tm;
	
	tm = gps_time / 604800I64;
	tm %= 1024;
	
	return (int)(tm);
}

void QKDTimer::GPSTimeToSystemTime(__int64 gps_time, SYSTEMTIME &sys_time)
{
	SYSTEMTIME t;
	FILETIME f;
	ULARGE_INTEGER l;
	__int64 tm;

	t.wYear = 1999;
	t.wMonth = 8;
	t.wDay = 22;
	t.wHour = 8;
	t.wMinute = 0;
	t.wSecond = 0;
	t.wMilliseconds = 0;
	
	SystemTimeToFileTime(&t,&f);
	memcpy(&l,&f,sizeof(FILETIME));
	
	tm = l.QuadPart;
	tm += gps_time*10000000I64;
	l.QuadPart = tm;
	
	memcpy(&f,&l,sizeof(FILETIME));
	FileTimeToSystemTime(&f,&sys_time);
	return;
}

__int64 QKDTimer::SystemTimeToGPSTime(SYSTEMTIME sys_time)
{
	FILETIME f;
	ULARGE_INTEGER l;
	SYSTEMTIME t;
	__int64 tm;

	t.wYear = 1999;
	t.wMonth = 8;
	t.wDay = 22;
	t.wHour = 8;
	t.wMinute = 0;
	t.wSecond = 0;
	t.wMilliseconds = 0;

	sys_time.wMilliseconds = 0;
	
	SystemTimeToFileTime(&sys_time,&f);
	memcpy(&l,&f,sizeof(FILETIME));
	
	tm = l.QuadPart;

	SystemTimeToFileTime(&t,&f);
	memcpy(&l,&f,sizeof(FILETIME));

	if(tm - (__int64)l.QuadPart < 0){
		return 0;
	}
	tm -= l.QuadPart;
	tm /=10000000I64;
	return tm;
}
