#include <stdarg.h>
#include <stdlib.h>
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
	FILE * fp = fopen("log.txt", "at");
	if( fp == NULL ) return;
	char buf[2048] = { 0, };
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
	fputs(buf, fp);
	fclose(fp);

	printf(buf);
}