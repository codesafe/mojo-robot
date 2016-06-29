#include "timer.h"

Timer *	Timer::instance = NULL;

Timer::Timer()
{

}

Timer::~Timer()
{

}


bool	Timer::init()
{

	return true;
}


void	Timer::uninit()
{
}

long	Timer::getticktime()
{
#if 1
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

std::chrono::system_clock::time_point Timer::gettime()
{
	std::chrono::system_clock::time_point tp;
	return std::chrono::system_clock::now();
}

