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

void Logger::log(const char* format, ...)
{
	char buf[2048] = { 0, };
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);

#ifdef TESTBUILD
	FILE * fp = fopen("log.txt", "at");
	if( fp == NULL ) return;
	fputs(buf, fp);
	fclose(fp);
#endif

	printf(buf);
}