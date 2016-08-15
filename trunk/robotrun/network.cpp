#include "network.h"
#include "commander.h"

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

// Parse recieved packet
bool	Network::read()
{
	// recieve and parse
	if( enable )
	{
		SocketBuffer buffer;
		bool ret = socket->recvpacket(&buffer);
		if( ret == true )
		{
			// packet parse
			parsepacket(&buffer);
		}
	}
	else
		return false;

	return true;
}

// send packet to server
bool	Network::write(char packet, char *data, int datasize)
{
	if(enable)
	{
		char buff[SOCKET_BUFFER];
		buff[0] = packet;
		memcpy(buff, data, datasize);
		return socket->sendpacket(buff, datasize+sizeof(char));
	}

	return false;
}

void	Network::parsepacket(SocketBuffer *buffer)
{
	char packet = (char&)*(buffer->buffer);
	int type = getpackettype(packet);

	int buffersize = buffer->totalsize - sizeof(char);
	Commander::getinstance()->addcommand( type, packet, buffer->buffer + sizeof(char), buffersize );
}

int	Network::getpackettype(char packet)
{
	int type = COMMAND_ANIMATION;
	switch(packet)
	{
		case DEVICERESET :
			type = COMMAND_DEVICE;
			break;

		case PLAYMOTION :
			type = COMMAND_ANIMATION;
			break;

		case WHEEL_FORWARD :
		case WHEEL_BACKWARD :
		case WHEEL_STOP :
		case WHEEL_TURNLEFT :
		case WHEEL_TURNRIGHT :
			type = COMMAND_WHEEL;
			break;

		case DISPLAY_PIC :
			type = COMMAND_DISPLAY;
			break;
	}

	return type;
}