#include "timer.h"

LARGE_INTEGER Timer::freq_;
LARGE_INTEGER Timer::counter_;

long	Timer::getticktime()
{
#if 0
	return clock();
#else
	#ifdef WIN32
		return clock();
	#else
		struct timeval tv;
		gettimeofday( &tv, NULL );
		return ( ( tv.tv_sec * 1000 ) + ( tv.tv_usec / 1000 ) );
	#endif
#endif
}

float Timer::getsecond()
{
#if 1
	return (float)clock() / (float)CLOCKS_PER_SEC;
#else

	#ifdef WIN32
		return (float)clock() / (float)CLOCKS_PER_SEC;
	#else
		return (float)getticktime() / 1000.f;
	#endif
#endif
}

double Timer::getCurrentTime()
{
#ifdef WIN32
	QueryPerformanceCounter(&counter_);
	QueryPerformanceFrequency(&freq_);
	return (double)counter_.QuadPart / (double)freq_.QuadPart * 1000.0;
#else
	struct timespec tv;
	clock_gettime( CLOCK_REALTIME, &tv);
	return ((double)tv.tv_sec*1000.0 + (double)tv.tv_nsec*0.001*0.001);
#endif
}
