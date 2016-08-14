﻿#ifndef _COMMANDER_
#define _COMMANDER_

#include "predef.h"
#include <deque>

struct Command
{
	int	type;
	char command;
	char data[SOCKET_BUFFER];

	Command()
	{
		command = 0;
		memset(data, 0, SOCKET_BUFFER);
	}
};


class Commander
{
public :
	static Commander * getinstance()
	{
		if (instance == NULL)
			instance = new Commander();
		return instance;
	}

	void	addcommand(int type, char command, char *data, int datalen);
	void	update();

private :	
	Commander();
	~Commander();
	
	static Commander*	instance;
	std::deque<Command>	commandlist;
};



#endif