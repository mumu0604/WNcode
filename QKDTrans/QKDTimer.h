/*
	$Id: QKDTimer.h 624 2012-06-29 04:22:44Z lwy $
	$URL: http://172.17.8.1/svn/QUESS/tags/QKDBench-2.2.1/QKDTimer.h $
*/

#ifndef __QKD_TIMER_H_
#define __QKD_TIMER_H_

#include <afx.h>

class QKDTimer
{
private:
	__int64 m_ShipTimeBase;
	__int64 m_GPSTimeBase;



public:
	QKDTimer();
	~QKDTimer();
	void ClearShipTime();
	void SetShipTime(__int64 time);
	__int64 GetShipTime();
	static __int64 GetShipTimeS();
	static __int64 GetShipTime(int day, int hour, int min, int sec, int ms);
	static __int64 GetShipTime(unsigned char *pBuf);
	static void GetShipTimeCode(__int64 time, unsigned char *pBuf);
	static int GetDayofShipTime(__int64 ship_time);
	static int GetHourofShipTime(__int64 ship_time);
	static int GetMinuteofShipTime(__int64 ship_time);
	static int GetSecondofShipTime(__int64 ship_time);
	static int GetMillisecondofShipTime(__int64 ship_time);
	static int GetLargeSecondofShipTime(__int64 ship_time);
	
	__int64 GetGPSTime();
	__int64 GetGPSTimeWithoutVerify();
	static __int64 GetGPSTime(int week, int sec);
	static __int64 GetGPSTime(unsigned char *pBuf);
	static void GetGPSTimeCode(__int64 time, unsigned char *pBuf);
	static int GetWeekofGPSTime(__int64 gps_time);
	static int GetSecondofGPSTime(__int64 gps_time);
	static void GPSTimeToSystemTime(__int64 gps_time, SYSTEMTIME &sys_time);
	static __int64 SystemTimeToGPSTime(SYSTEMTIME sys_time);
	static __int64 GetGPSTimeNowday();
	int m_GPSMode;
};


#endif
