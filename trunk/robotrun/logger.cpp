#include <stdarg.h>
#include <stdlib.h>
#include "predef.h"
#include "logger.h"
#include "network.h"

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

#ifdef TESTBUILD
	if (Network::getinstance()->getenable())
	{
#define CLIENT_LOG_INFO		0x0a
#define CLIENT_LOG_WARN		0x0b
#define CLIENT_LOG_ERR		0x0c

		char packet = CLIENT_LOG_INFO;
		if(logtype == LOG_INFO)
			packet = CLIENT_LOG_INFO;
		else if (logtype == LOG_WARN)
			packet = CLIENT_LOG_WARN;
		else if (logtype == LOG_ERR)
			packet = CLIENT_LOG_ERR;

		Network::getinstance()->write(packet, buf, strlen(buf));
	}
#endif
}