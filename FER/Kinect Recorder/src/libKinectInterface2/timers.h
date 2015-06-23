#pragma once

/********************************************************************
	created:	2014/09/30
	filename: 	libKinectInterface2\timers.hpp
	file path:	libKinectInterface2
	file base:	timers
	file ext:	hpp
	author:		Gregorij Kurillo
	
	purpose:	Collection of timers
	comment:	This may need to be moved to some general purpose library
*********************************************************************/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#include <windows.h>
#include <time.h>
#include <sphelper.h>
#include <iostream>
using namespace std;



/* 
* Brief Execution time measurement. User thread time and kernel timespan elapsed 
* between consecutive tic() and toc() calls is stored
*/
struct ThreadTimes
{
	ThreadTimes (void) { thread = GetCurrentThread (); LARGE_INTEGER freq; QueryPerformanceFrequency (&freq); perfFrequency = static_cast<double>(freq.QuadPart) / 1000.0; }

	void tic (void) { GetThreadTimes (thread, &ticTimes.creationTime, &ticTimes.exitTime, &ticTimes.kernelTime, &ticTimes.userTime); ticClock = clock (); /*DWORD_PTR oldmask = SetThreadAffinityMask (thread, 0);*/ QueryPerformanceCounter (&perfStart); /*SetThreadAffinityMask (thread, oldmask);*/ }
	void toc (void) { GetThreadTimes (thread, &tocTimes.creationTime, &tocTimes.exitTime, &tocTimes.kernelTime, &tocTimes.userTime); tocClock = clock (); /*DWORD_PTR oldmask = SetThreadAffinityMask (thread, 0);*/ QueryPerformanceCounter (&perfStop); /*SetThreadAffinityMask (thread, oldmask);*/ }

	// thread time in milliseconds
	int userTimeMS (void) const { return int (userTimeNS () / 10000); }         ///< user thread timespan in milliseconds
	int kernelTimeMS (void) const { return int (kernelTimeNS () / 10000); }     ///< kernel thread timespan in milliseconds
	int clockTimeMS (void) const { return static_cast<int>((tocClock - ticClock) / static_cast<double>(CLOCKS_PER_SEC) * 1000); }
	double perfTimeMS (void) const { return ((perfStop.QuadPart - perfStart.QuadPart) / perfFrequency); }

protected:
	struct TimeInfo
	{
		FILETIME creationTime, exitTime, kernelTime, userTime;
	};


	HANDLE thread;
	TimeInfo ticTimes;
	TimeInfo tocTimes;
	clock_t ticClock;
	clock_t tocClock;

	LARGE_INTEGER perfStart;
	LARGE_INTEGER perfStop;
	double perfFrequency;


	LONGLONG userTimeNS (void) const { return *(LONGLONG *) &tocTimes.userTime - *(LONGLONG *) &ticTimes.userTime; }
	LONGLONG kernelTimeNS (void) const { return *(LONGLONG *) &tocTimes.kernelTime - *(LONGLONG *) &ticTimes.kernelTime; }
};

inline ostream &operator << (ostream &os, const ThreadTimes &tt)
{
	return os << tt.userTimeMS () << " + " << tt.kernelTimeMS () << " = " << tt.clockTimeMS () << " (" << tt.perfTimeMS () << ") ms.";
}


/**
*  Unix timer class
*/
struct UnixTimer
{
  UnixTimer (void);

  bool operator == (const UnixTimer &ut) { return seconds == ut.seconds && microseconds == ut.microseconds; }
  bool operator < (const UnixTimer &ut) { if (seconds == ut.seconds) return microseconds < ut.microseconds; return seconds < ut.seconds; }

  LONG seconds;
  LONG microseconds;

  void getCurrentTime(void);

};

/**
* Stop watch for real time (double)
*/
class CStopWatch {

public:
	typedef struct {
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
} stopWatch;


private:
    stopWatch timer;
    LARGE_INTEGER frequency;
    double LIToSecs( LARGE_INTEGER & L) ;
public:
	CStopWatch();
	~CStopWatch() {};
    void startTimer( );
	void resetTimer( );
    void stopTimer( );	
    double getElapsedTime() ;		///< Returns time in seconds
};

