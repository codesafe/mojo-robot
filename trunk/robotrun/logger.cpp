#include <stdarg.h>
#include <stdlib.h>
#include "predef.h"
#include "logger.h"

Logger * Logger::instance = NULL;

Logger::Logger()
{
}

Logger::~Logger()
{

}

void Logger::log(int logtype, const char* format, ...)
{
	bool consolelog_enable = (CONSOLE_LOG & logtype) == 1 ? false : true;
	bool filelog_enable = (FILE_LOG & logtype) == 0 ? false : true;

	char buf[2048] = { 0, };
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);

	if (filelog_enable)
	{
		FILE * fp = fopen("log.txt", "at");
		if (fp == NULL) return;
		fputs(buf, fp);
		fclose(fp);
	}

	if(consolelog_enable)
		printf(buf);
}