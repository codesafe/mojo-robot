#ifndef _DISPLAY_
#define _DISPLAY_

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

#include "part.h"

#ifdef _WIN32
#include "pthread.h"
#endif

#define	MAX_PACKET	128

struct DisplayInfo
{
	int			side;
	uint8_t		command;
	uint8_t		param[MAX_PACKET];
	int			length;

	DisplayInfo()
	{
		side = 0;
		command = 0;
		memset(param, 0, MAX_PACKET);
		length = 0;
	}
};

class Display : public Part
{
public :
	Display();
	~Display();

	bool	init(XMLNode node);
	void	uninit();
	bool	reset();

	bool	sendcommand(uint16_t command, uint16_t param);
	bool	recvcommand(uint16_t command, uint16_t &param);

	void	addcommandlist(uint8_t command, uint8_t *param = NULL, int length = 0);
	int		flushcommandlist();
	
	static void * thread_fn(void *arg);

private:

	int			side;		// 어느쪽 눈?
	uint8_t		storage;
	uint8_t		rotation;

	pthread_t				threadid;

	static bool exitthread[EYE_MAX];
	static bool actionTrigger[EYE_MAX];
	static pthread_mutex_t mutex_lock[EYE_MAX];
	static std::vector<DisplayInfo>	displayinfolist[EYE_MAX];

};


#endif
