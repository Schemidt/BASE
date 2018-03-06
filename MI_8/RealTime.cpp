/*!
\file
\brief Реализует реальное время / Определение
*/
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "RealTime.h"
#include "Shared.h"

void DelayMs(int ms);

REALTIME rt;

CRITICALSECTIONS crts;

CRITICALSECTIONS::CRITICALSECTIONS() {
	InitializeCriticalSection(&cs); 
}

CRITICALSECTIONS::~CRITICALSECTIONS()
{
	DeleteCriticalSection(&cs);
}

void CRITICALSECTIONS::Lock() {
		EnterCriticalSection(&cs);
}

void CRITICALSECTIONS::Unlock() {
	LeaveCriticalSection(&cs);
};


REALTIME::REALTIME()
{
	timeS = 0;
}

double GetTimeMs()
{
	static long double Sc = 0;
	LARGE_INTEGER cnt;
	if(Sc == 0){
		LARGE_INTEGER fr;
		if(QueryPerformanceFrequency(&fr) == 0)
			return GetTickCount();
		Sc = 1000/(long double)fr.QuadPart;
	}
	QueryPerformanceCounter(&cnt);
	return (long double)cnt.QuadPart*Sc;
}


void CALLBACK RealTimeFunc(UINT,UINT,DWORD st,DWORD,DWORD)
{
	static int flag=1;
	double tm0 = GetTimeMs();

	crts.Lock();

								// Обмен через общую память
//	rt.pExchOK = NetReceiveVoice();

	memcpy((void*)&soundread, &shaData->sr, sizeof(SOUNDREAD));

	rt.SetEvent = timeSetEvent(rt.step,1,RealTimeFunc,rt.step,TIME_ONESHOT);

	crts.Unlock();
	double tm1 = GetTimeMs();
	rt.dT = tm1 - tm0;
}

void InitRealTime(DWORD  STEP)
{
	rt.step = STEP;
	rt.Step = rt.step * 0.001;
	rt.timeS = 0;
	rt.SetEvent = timeSetEvent(rt.step,1,RealTimeFunc,rt.step,TIME_ONESHOT);
}

void StopRealTime()
{
	if(rt.SetEvent){
		timeKillEvent(rt.SetEvent);
		rt.SetEvent = 0;
	}
}
