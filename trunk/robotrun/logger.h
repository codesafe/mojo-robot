#ifndef _LOGGER_
#define _LOGGER_

#include <stdio.h>

class Logger
{
public:
	static Logger *	instance;
	static Logger *getInstance()
	{
		if (instance == NULL)
			instance = new Logger();
		return instance;
	};

	void log(const char* format, ...);

private:
	Logger();
	~Logger();

};






#endif