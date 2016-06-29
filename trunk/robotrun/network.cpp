#include "network.h"


Network *	Network::instance = NULL;

Network::Network()
{
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