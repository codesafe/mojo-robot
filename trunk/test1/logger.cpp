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
	char buf[1024] = { 0, };
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
	//printf(buf);
	//fwrite(buf, 1, 1024, fp);
	fputs(buf, fp);

	fclose(fp);
}