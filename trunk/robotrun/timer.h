#ifndef _TIMER_
#define _TIMER_

#include "predef.h"


#ifdef WIN32
	#include <ctime>
	#include <chrono>
#else
	#include <stdio.h>
	#include <fcntl.h>
	#include <string.h>
	#include <unistd.h>
	#include <termios.h>
	#include <time.h>
	#include <sys/time.h>
#endif

class Timer
{
public:

	static long	getticktime();
	static float	getsecond();
	static double	getCurrentTime();

#ifdef WIN32
	static LARGE_INTEGER freq_, counter_;
#endif

private:

	Timer() {};
	~Timer() {};
};


#endif