#ifndef _DEVICE_
#define _DEVICE_

#include "predef.h"

#define NEWGROUPWRITE


// device init / setup 

struct GROUPWRITE
{
	uint8_t id;
	uint16_t command;
	uint16_t param;
	uint16_t data_length;

};


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
	bool	recv(uint8_t id, uint16_t command, uint16_t commandsize, uint16_t &param);

	bool	addsendqueue(uint8_t id, uint16_t command, uint16_t param);
	bool	sendqueue();

	bool	addrecvqueue(uint8_t id, uint16_t command, uint16_t commandlen);
	bool	recvqueue();
	int		recvqueuedata(uint8_t id, uint16_t command, uint16_t commandlen);
	void	clearrecvqueue();

	//////////////////////////////////////////////////////////////////////////	For e-ink display

	int		recvcommand();
	int		sendcommand(uint8_t command, uint8_t *param = NULL, int length = 0);

	static void	Sleep(int t);

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
	
#ifndef NEWGROUPWRITE
	// this is for groupwrite position
	std::map<int, dynamixel::GroupSyncWrite*>	groupwritelist;
#endif

	std::vector<GROUPWRITE>	groupwriteinfolist;

	//////////////////////////////////////////////////////////////////////////	For e-ink display

	bool	enabledisplayport;
	dynamixel::PortHandler *displayportHandler;

	static Device *	instance;
};


#endif