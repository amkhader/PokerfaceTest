#include "libKinectInterface2.h"
#include "timers.h"



UnixTimer::UnixTimer() :
	seconds (0), 
	microseconds (0) 
{
	getCurrentTime(); 
}



void UnixTimer :: getCurrentTime (void)
	{
	  FILETIME ft;
	  LARGE_INTEGER date, adjust;
	  static struct timeval base_time = { 0, 0 };
	  static DWORD base_tickcount = 0;

	  // Get System file time
	  GetSystemTimeAsFileTime (&ft);

	  date.HighPart = ft.dwHighDateTime;
	  date.LowPart = ft.dwLowDateTime;

	  // 11644473600000 is the timestamp of January 1, 1601 (UTC), when
	  // FILETIME started.
	  // 100-nanoseconds = milliseconds * 10000
	  adjust.QuadPart = 11644473600000 * 10000;

	  // removes the diff between 1970 and 1601
	  date.QuadPart -= adjust.QuadPart;

	  // converts back from 100-nanoseconds to seconds and microseconds
	  seconds =  (LONG)(date.QuadPart / 10000000);
	  adjust.QuadPart = seconds;
	  adjust.QuadPart *= 10000000;
	  microseconds = (LONG)((date.QuadPart - adjust.QuadPart) / 10);

	//  SYSTEMTIME now_st;
	//  GetSystemTime (&now_st);
	}


double CStopWatch::LIToSecs( LARGE_INTEGER & L) {
    return ((double)L.QuadPart /(double)frequency.QuadPart) ;
}

CStopWatch::CStopWatch(){
    timer.start.QuadPart=0;
    timer.stop.QuadPart=0;
    QueryPerformanceFrequency( &frequency ) ;
}

void CStopWatch::startTimer( ) {
    QueryPerformanceCounter(&timer.start) ;
}

void CStopWatch::resetTimer( ) {
	timer.start.QuadPart=0;
    timer.stop.QuadPart=0;
    QueryPerformanceCounter(&timer.start) ;
}

void CStopWatch::stopTimer( ) {
    QueryPerformanceCounter(&timer.stop) ;
}

double CStopWatch::getElapsedTime() {
    LARGE_INTEGER time;
    time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
    return LIToSecs( time) ;
}