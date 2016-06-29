#ifndef _DEVICE_
#define _DEVICE_

#include "predef.h"

// device init / setup 

class Device
{
public :
	static Device *getInstance()
	{
		if (instance == NULL)
			instance = new Device();
		return instance;
	};

	bool	init();
	void	uninit();
	bool	send(uint8_t id, uint16_t command, uint16_t commandsize, uint16_t param);
	bool	recv(uint8_t id, uint16_t command, uint16_t &param);

	bool	addsendqueue(uint8_t id, uint16_t command, uint16_t param);
	bool	sendqueue();

	bool	addrecvqueue(uint8_t id, uint16_t command, uint16_t commandlen);
	bool	recvqueue();
	int		recvqueuedata(uint8_t id, uint16_t command, uint16_t commandlen);
	void	clearrecvqueue();

	//////////////////////////////////////////////////////////////////////////	For e-ink display

	int		recvcommand();
	int		sendcommand(uint8_t command, uint8_t *param = NULL, int length = 0);

private :
	Device();
	~Device();

	bool initdevice(std::string part);
	unsigned char verify(const void * ptr, int n);

private :
	bool	enablejointport;

	dynamixel::PortHandler *portHandler;
	dynamixel::PacketHandler *packetHandler;

	dynamixel::GroupBulkRead *groupRead;
	// this is for groupwrite position
	std::map<int, dynamixel::GroupSyncWrite*>	groupwritelist;

	//////////////////////////////////////////////////////////////////////////	For e-ink display

	bool	enabledisplayport;
	dynamixel::PortHandler *displayportHandler;

	static Device *	instance;
};


#endif