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
	static Timer *getInstance()
	{
		if (instance == NULL)
			instance = new Timer();
		return instance;
	};

	bool	init();
	void	uninit();

	long	getticktime();
	float	getsecond();
	double	getCurrentTime();

private:

	Timer();
	~Timer();

	static Timer *	instance;

#ifdef WIN32
	LARGE_INTEGER freq_, counter_;
#endif
};


#endif