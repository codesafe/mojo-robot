#ifndef _NETWORK_
#define _NETWORK_

#include "socket.h"


class Network
{
public :
	static Network * getinstance()
	{
		if (instance == NULL)
			instance = new Network();
		return instance;
	}

	void	init();
	void	uninit();

private :
	Network();
	~Network();

	static Network *	instance;
	Socket	*	socket;
};


#endif