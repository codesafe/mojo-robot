#ifndef _TIMER_
#define _TIMER_

#include "predef.h"

#if 1
#include <ctime>
#include <chrono>
#else

	#ifdef WIN32
		#include <ctime>
		#include <chrono>
	#else
		#include <sys/time.h>
	#endif

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

	std::chrono::system_clock::time_point gettime();

private:

	Timer();
	~Timer();

	static Timer *	instance;

};


#endif