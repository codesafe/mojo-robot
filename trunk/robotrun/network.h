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
	bool	connect();

	void	update();
	bool	read();
	bool	write();

private :
	Network();
	~Network();

	static Network *	instance;

	bool		enable;
	Socket	*	socket;
};


#endif