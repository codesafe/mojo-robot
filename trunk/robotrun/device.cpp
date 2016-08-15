#ifdef WIN32
#include <assert.h>
#endif
#include "device.h"


#define NEWGROUPWRITE

Device *	Device::instance = NULL;

//	모든 디바이스 Data send / recieve
// 순서가 맞아야 함. (이동을 시작하고 speed를 설정하면 안됨)
#define ORDERNUM	2
const int order[ORDERNUM] = { MOVE_SPEED, DEST_POSITION };

Device::Device()
{
}

Device::~Device()
{
}

bool	Device::init()
{
	enablejointport = initdevice("joint");
	//enabledisplayport = initdevice("display");

	return true;
}

bool	Device::initdevice(std::string part)
{

	if( part == "joint" )
	{
		//////////////////////////////////////////////////////////////////	Joint and Wheel

#ifdef WIN32
		portHandler = dynamixel::PortHandler::getPortHandler(MemDB::getInstance()->getValue("jointdevicename").c_str());
		Logger::getInstance()->log("Try to getHandler %s\n", MemDB::getInstance()->getValue("jointdevicename").c_str());
#else
		portHandler = dynamixel::PortHandler::getPortHandler(MemDB::getInstance()->getValue("linuxjointdevicename").c_str());
		Logger::getInstance()->log("Try to getHandler %s\n", MemDB::getInstance()->getValue("linuxjointdevicename").c_str());
#endif

		packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

		if (portHandler->openPort())
		{
			Logger::getInstance()->log("Succeeded to open the port!\n");
		}
		else
		{
			Logger::getInstance()->log("Failed to open the port!\n");
			Logger::getInstance()->log("Press any key to terminate...\n");
			return false;
		}

		// Set port baudrate
		int baudrate = MemDB::getInstance()->getIntValue("jointbaudrate");
		if (portHandler->setBaudRate(baudrate))
		{
			Logger::getInstance()->log("Succeeded to change the baudrate : %d !\n", baudrate);
		}
		else
		{
			Logger::getInstance()->log("Failed to change the baudrate!\n");
			Logger::getInstance()->log("Press any key to terminate...\n");
			return false;
		}

		groupRead = new dynamixel::GroupBulkRead(portHandler, packetHandler);

		// group write init
		for(int i=0; i<ORDERNUM; i++)
		{
			dynamixel::GroupSyncWrite *gw = new dynamixel::GroupSyncWrite(portHandler, packetHandler, order[i], 2);
			groupwritelist.insert(std::make_pair(order[i], gw));
		}
	}
	else if( part == "display" )
	{
		//////////////////////////////////////////////////////////////////	for e-ink display

#ifdef WIN32
		displayportHandler = dynamixel::PortHandler::getPortHandler(MemDB::getInstance()->getValue("displaydevicename").c_str());
		Logger::getInstance()->log("Try to getHandler display %s\n", MemDB::getInstance()->getValue("displaydevicename").c_str());
		assert(displayportHandler);
#else
		displayportHandler = dynamixel::PortHandler::getPortHandler(MemDB::getInstance()->getValue("linuxdisplaydevicename").c_str());
		Logger::getInstance()->log("Try to getHandler %s\n", MemDB::getInstance()->getValue("linuxdisplaydevicename").c_str());
#endif

		if (displayportHandler->openPort())
		{
			Logger::getInstance()->log("Succeeded to open the port!\n");
		}
		else
		{
			Logger::getInstance()->log("Failed to open the port!\n");
			Logger::getInstance()->log("Press any key to terminate...\n");
			return false;
		}

		// Set port baudrate
		int dispbaudrate = MemDB::getInstance()->getIntValue("displaybaudrate");
		if (displayportHandler->setBaudRate(dispbaudrate))
		{
			Logger::getInstance()->log("Succeeded to change the dispbaudrate : %d !\n", dispbaudrate);
		}
		else
		{
			Logger::getInstance()->log("Failed to change the dispbaudrate!\n");
			Logger::getInstance()->log("Press any key to terminate...\n");
			return false;
		}
	}

	return true;
}


void	Device::uninit()
{
	for(int i=0; i<ORDERNUM; i++)
		delete groupwritelist[order[i]];

	delete groupRead;
	portHandler->closePort();
}

// single command
bool	Device::send(uint8_t id, uint16_t command, uint16_t commandsize, uint16_t param)
{
	if( enablejointport == false ) return false;

	uint8_t dxl_error = 0;
	int dxl_comm_result = COMM_TX_FAIL;

	if(commandsize == 1)
	{
		uint8_t p = (uint8_t)param;
		dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, id, command, p, &dxl_error);
	}
	else
		dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, id, command, param, &dxl_error);

	if (dxl_comm_result != COMM_SUCCESS)
	{
		Logger::getInstance()->log("send not success %d \n", dxl_comm_result);
		packetHandler->printTxRxResult(dxl_comm_result);
		return false;
	}
	else if (dxl_error != 0)
	{
		Logger::getInstance()->log("send error %d \n", dxl_error);
		packetHandler->printRxPacketError(dxl_error);
		return false;
	}

	return true;
}

bool	Device::recv(uint8_t id, uint16_t command, uint16_t commandsize, uint16_t &param)
{
	if( enablejointport == false ) return false;

	uint8_t dxl_error = 0;
	int dxl_comm_result = COMM_TX_FAIL;

	if(commandsize == 1)
	{
		uint8_t p = (uint8_t)param;
		dxl_comm_result = packetHandler->read1ByteTxRx(portHandler, id, command, &p, &dxl_error);
		param = p;
	}
	else
	{
		dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, id, command, &param, &dxl_error);
	}

	if (dxl_comm_result != COMM_SUCCESS)
	{
		Logger::getInstance()->log("recv not success %d \n", dxl_comm_result);
		packetHandler->printTxRxResult(dxl_comm_result);
		return false;
	}
	else if( dxl_error != 0 )
	{
		Logger::getInstance()->log("recv error %d \n", dxl_error);
		packetHandler->printRxPacketError(dxl_error);
		return false;
	}
	return true;
}

//
//	Group Write to Device
//	명령들을 queue에 쌓아 놓는다
//
bool	Device::addsendqueue(uint8_t id, uint16_t command, uint16_t param)
{
	if( enablejointport == false ) return false;

#ifdef NEWGROUPWRITE
	GROUPWRITE info;
	info.id = id;
	info.command = command;
	info.param = param;
	info.data_length = 2;		// ??
	groupwriteinfolist.push_back(info);

#else
	uint8_t params[2];
	params[0] = DXL_LOBYTE(DXL_LOWORD(param));
	params[1] = DXL_HIBYTE(DXL_LOWORD(param));

	bool dxl_addparam_result = false;
	dxl_addparam_result = groupwritelist[command]->addParam(id, params);
	return dxl_addparam_result;
#endif

	return true;
}

//
//	Group Write to Device
//	쌓아논 명령들을 디바이스에 전송
//
bool	Device::sendqueue()
{
	if( enablejointport == false ) return false;

#ifdef NEWGROUPWRITE
	int dxl_comm_result = COMM_TX_FAIL;
	std::vector<dynamixel::GroupSyncWrite*>	grouplist;

	// group write 준비
	for (size_t i=0; i<groupwriteinfolist.size(); i++)
	{
		dynamixel::GroupSyncWrite *gw = new dynamixel::GroupSyncWrite(portHandler, packetHandler, groupwriteinfolist[i].command, groupwriteinfolist[i].data_length);

		uint8_t params[2];
		params[0] = DXL_LOBYTE(DXL_LOWORD(groupwriteinfolist[i].param));
		params[1] = DXL_HIBYTE(DXL_LOWORD(groupwriteinfolist[i].param));

		bool dxl_addparam_result = false;
		dxl_addparam_result = gw->addParam(groupwriteinfolist[i].id, params);

		if( dxl_addparam_result == false )
		{
			Logger::getInstance()->log("Group write add param error. %d , %d\n", groupwriteinfolist[i].id, groupwriteinfolist[i].command);
			delete gw;
			return false;
		}

		grouplist.push_back(gw);
	}
	groupwriteinfolist.clear();

	// send to device
	bool ret = true;
	for(size_t i=0; i<grouplist.size(); i++)
	{
		int dxl_comm_result = COMM_TX_FAIL;
		dxl_comm_result = grouplist[i]->txPacket();
		if (dxl_comm_result != COMM_SUCCESS)
		{
			Logger::getInstance()->log("sendqueue not success %d \n", dxl_comm_result);
			packetHandler->printTxRxResult(dxl_comm_result);
			ret = false;
		}
	}

	// clear
	for(size_t i=0; i<grouplist.size(); i++)
		delete grouplist[i];

	grouplist.clear();
	return ret;
#else
	int dxl_comm_result = COMM_TX_FAIL;
	for( int i=0; i<2; i++)
	{
		dxl_comm_result = groupwritelist[order[i]]->txPacket();
		if (dxl_comm_result != COMM_SUCCESS)
		{
			Logger::getInstance()->log("sendqueue not success %d \n", dxl_comm_result);
			packetHandler->printTxRxResult(dxl_comm_result);
			return false;
		}
	}

	std::map<int, dynamixel::GroupSyncWrite*>::iterator it = groupwritelist.begin();
	for( ; it != groupwritelist.end(); it++)
		it->second->clearParam();

	return true;
#endif
}

// 가져올 데이터들 등록
bool	Device::addrecvqueue(uint8_t id, uint16_t command, uint16_t commandlen)
{
	if( enablejointport == false ) return false;

	bool result = groupRead->addParam(id, command, commandlen);
	if (result != true)
	{
		Logger::getInstance()->log("%d : grouBulkRead addparam failed\n", id);
		return false;
	}

	return true;
}

// 데이터 읽기 위하여 모두 가져와 패킷 날림
bool	Device::recvqueue()
{
	if( enablejointport == false ) return false;

	int dxl_comm_result = groupRead->txRxPacket();
	if (dxl_comm_result != COMM_SUCCESS) packetHandler->printTxRxResult(dxl_comm_result);

	return true;
}

// 위에서 가져온 데이터 얻기
int		Device::recvqueuedata(uint8_t id, uint16_t command, uint16_t commandlen)
{
	if( enablejointport == false ) return 0;

	bool result = groupRead->isAvailable(id, command, commandlen);
	if (result != true)
	{
		//fprintf(stderr, "[ID:%03d] groupBulkRead getdata failed", DXL2_ID);
		return -1;
	}

	int recvdata = groupRead->getData(id, command, commandlen);
	return recvdata;
}

void	Device::clearrecvqueue()
{
	if( enablejointport == false ) return;

	groupRead->clearParam();
}

//////////////////////////////////////////////////////////////////////////

/// for i-ink display

#include "e-ink.h"

int		Device::recvcommand()
{
	if( enabledisplayport == false ) return 0;

	int     result = COMM_TX_FAIL;
	uint8_t checksum = 0;
	uint8_t rx_length = 0;		// 총 수신 량
	uint8_t rxpacket[RXPACKET_MAX_LEN] = { 0, };

	displayportHandler->setPacketTimeout(5000.0);

	while (true)
	{
		rx_length += displayportHandler->readPort(&rxpacket[rx_length], RXPACKET_MAX_LEN - rx_length);
		if (rx_length > 0)
		{
			if( rx_length == 2 )
			{
				if( rxpacket[0] == 'O' && rxpacket[1] == 'K' )
				{
					result = COMM_SUCCESS;
					break;
				}
			}
			else if( rx_length > 2 )
			{
				Logger::getInstance()->log("Display Error ---> %s\n",(const char *)rxpacket);
				result = COMM_RX_CORRUPT;
				break;
			}
		}
		else
		{
			// check timeout
			if (displayportHandler->isPacketTimeout() == true)
			{
				if (rx_length == 0)
				{
					result = COMM_RX_TIMEOUT;
					Logger::getInstance()->log("Display Error ---> Time Out!!\n");
				}
				else
				{
					result = COMM_RX_CORRUPT;
				}
				break;
			}
		}
	}
	displayportHandler->is_using_ = false;

	return result;
}

// calc parity bit
unsigned char Device::verify(const void * ptr, int n)
{
	int i;
	unsigned char * p = (unsigned char *)ptr;
	unsigned char result;

	for (i = 0, result = 0; i < n; i++)
	{
		result ^= p[i];
	}

	return result;
}

int		Device::sendcommand(uint8_t command, uint8_t *param, int length)
{
	if( enabledisplayport == false ) return 0;

	uint8_t txpacket[128] = { 0, };
	uint8_t checksum = 0;
	uint8_t total_packet_length = 0;
	uint8_t written_packet_length = 0;

	if (displayportHandler->is_using_)
		return COMM_PORT_BUSY;

	displayportHandler->is_using_ = true;

	// 명령어에 따라 버퍼 설정
	switch (command)
	{
		case CMD_HANDSHAKE :
			memcpy(txpacket, _cmd_handshake, 8);
			total_packet_length = 8;
			break;

		case CMD_UPDATE :
			memcpy(txpacket, _cmd_update, 8);
			total_packet_length = 8;
			break;

		case CMD_MEMORYMODE :
			memcpy(txpacket, _cmd_set_storage, 9);
			total_packet_length = 9;
			txpacket[4] = param[0];
			break;

		case CMD_DRAW_BITMAP :
			// header
			memcpy(txpacket, _cmd_draw_bmp_head, 8);
			total_packet_length += 8;

			// name
			memcpy(txpacket+total_packet_length, param, length);
			total_packet_length += length;

			// end of name
			txpacket[total_packet_length] = 0;
			total_packet_length ++;

			// end
			memcpy(txpacket+total_packet_length, _cmd_draw_bmp_end, 4);
			total_packet_length += 4;

			// packet size
			txpacket[2] = (uint8_t)total_packet_length+1;
			break;

		default:
			return COMM_TX_FAIL;
	}

	// make paritybit
	checksum = verify(txpacket, total_packet_length);
	txpacket[total_packet_length] = checksum;
	total_packet_length++;

	// tx packet
	displayportHandler->clearPort();

	// 패킷 발사
	written_packet_length = displayportHandler->writePort(txpacket, total_packet_length);
	if (total_packet_length != written_packet_length)
	{
		displayportHandler->is_using_ = false;
		return COMM_TX_FAIL;
	}

	// 결과를 받아야 하나?? --> ok 메시지?
	return recvcommand();
}
