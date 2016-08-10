#include "network.h"


Network *	Network::instance = NULL;

Network::Network()
{
	enable = false;
	socket = NULL;
}

Network::~Network()
{

}

void	Network::init()
{
	socket = new Socket();
	socket->init();
}

void	Network::uninit()
{
	socket->uninit();
	delete socket;
	socket = NULL;
}

bool	Network::connect()
{
	enable = socket->connect();
	Logger::getInstance()->log("socket connect : %s\n", enable ? "success" : "fail");

	return enable;
}

void	Network::update()
{
	if( enable )
	{
		bool ret = socket->update();
		if( ret == false)
		{
			Logger::getInstance()->log("socket disconnected\n");
			enable = false;
		}
	}
}


bool	Network::read()
{
	// recieve and parse
	if( enable )
	{
		
	}
	else
	{
		Logger::getInstance()->log("socket not connected\n");
		return false;
	}

	return true;
}

bool	Network::write()
{

	return true;
}