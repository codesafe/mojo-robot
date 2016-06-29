#include "joint.h"
#include "device.h"

Joint::Joint()
{
	torque = false;
	initpos = 0;
	cwlimit = 0;
	ccwlimit = 0;
}

Joint::~Joint()
{
}

bool	Joint::init(XMLNode node)
{
	Part::init(node);

	// load detail information
	for (int i = 0; i < node.nChildNode(); i++)
	{
		XMLNode child = node.getChildNode(i);

		const char *name = child.getName();
		const char *value = child.getText();

		if(strcmp(name, "id") == 0)
		{
			id = xmltoi(value);
		}
		else if(strcmp(name, "cwlimit") == 0)
		{
			cwlimit = (uint16_t)xmltoi(value);
		}
		else if(strcmp(name, "ccwlimit") == 0)
		{
			ccwlimit = (uint16_t)xmltoi(value);
		}
		else if(strcmp(name, "initpos") == 0)
		{
			initpos = (uint16_t)xmltoi(value);
		}
		else if(strcmp(name, "torque") == 0)
		{
			torque = (xmltoi(value) == 1 ? true : false);
		}
	}

	return true;
}

void	Joint::uninit()
{
	bool ret = false;
	if( torque )
	{
		ret = Device::getInstance()->send(id, TORQUEMODE, 1, DEVICE_DISABLE);
		if( ret == false )
			Logger::getInstance()->log("%d : torque set error!! \n", id);
	}
}

bool	Joint::reset()
{
	// TODO. 초기화 에러 유무 검사
	// TODO. 임계각 같은 초기 설정 값 일치하는가 검사
	// TODO. 기본 포지션을 이동
	uint16_t param = 0;
	bool ret = false;
	
	// enable torque mode
	if( torque )
	{
		ret = Device::getInstance()->send(id, TORQUEMODE, 1, DEVICE_ENABLE);
		if( ret == false )
		{
			Logger::getInstance()->log("%d : torque set error!! \n", id);
			return false;
		}
	}

	// check and adjust cwlimit
	ret = Device::getInstance()->recv(id, CW_LIMIT_ANGLE, param);
	if( ret == false )
	{
		Logger::getInstance()->log("%d : cwlimit recv error!! \n", id);
		return false;
	}
	if(DXL2DEGREE(param) != cwlimit)
	{
		Logger::getInstance()->log("%d : cwlimit is not match : cwlimit : %d, recv cwlimit : \n", id, cwlimit, param);

		ret = Device::getInstance()->send(id, CW_LIMIT_ANGLE, 2, cwlimit);
		if( ret == true )
		{
			Logger::getInstance()->log("%d : adjust cwlimit to %d \n", id, cwlimit);
		}
		else if( ret == false )
		{
			Logger::getInstance()->log("%d : set cwlimit send error!! \n", id);
			return false;
		}
	}
	else
	{
		Logger::getInstance()->log("%d : cwlimit is good!\n", id);
	}

	// check and adjust ccwlimit
	param = 0;
	ret = Device::getInstance()->recv(id, CCW_LIMIT_ANGLE, param);
	if( ret == false )
	{
		Logger::getInstance()->log("%d : ccwlimit recv error!! \n", id);
		return false;
	}

	if( DXL2DEGREE(param) != ccwlimit)
	{
		Logger::getInstance()->log("%d : ccwlimit is not match : ccwlimit : %d, recv ccwlimit : %d\n", id, ccwlimit, DXL2DEGREE(param));

		ret = Device::getInstance()->send(id, CCW_LIMIT_ANGLE, 2, DEGREE2DXL(ccwlimit));
		if( ret == true )
		{
			Logger::getInstance()->log("%d : adjust ccwlimit to %d \n", id, ccwlimit);
		}
		else if( ret == false )
		{
			Logger::getInstance()->log("%d : set ccwlimit send error!! \n", id);
			return false;
		}
	}
	else
	{
		Logger::getInstance()->log("%d : ccwlimit is good!\n", id);
	}

/*
	// set to init position
	ret = Device::getInstance()->send(id, DEST_POSITION, DEGREE2DXL(initpos));
	if( ret == true )
	{
		Logger::getInstance()->log("%d : set to init pos %d \n",id, initpos);
	}
	else if( ret == false )
	{
		Logger::getInstance()->log("%d : set to init pos error \n",id);
		return false;
	}
*/
	ret = Device::getInstance()->addsendqueue(id, DEST_POSITION, DEGREE2DXL(initpos));
	ret = Device::getInstance()->addsendqueue(id, MOVE_SPEED, 0);


	return true;
}

bool	Joint::addsendcommand(uint16_t command, uint16_t param) 
{ 
	return Device::getInstance()->addsendqueue(id, command, param);
}

bool	Joint::sendcommand(uint16_t command , uint16_t param)
{
	return Device::getInstance()->addsendqueue(id, command, param);
}

bool	Joint::recvcommand(uint16_t command, uint16_t &param)
{
	return Device::getInstance()->recv(id, command, param);
}