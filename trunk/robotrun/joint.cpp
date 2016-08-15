﻿#include "joint.h"
#include "device.h"

Joint::Joint()
{
	torque = false;
	initpos = 0;
	cwlimit = 0;
	ccwlimit = 0;
	p_param = -1;
	i_param = -1;
	d_param = -1;
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
		else if (strcmp(name, "p-param") == 0)
		{
			p_param = (uint16_t)xmltoi(value);
		}
		else if (strcmp(name, "i-param") == 0)
		{
			i_param = (uint16_t)xmltoi(value);
		}
		else if (strcmp(name, "d-param") == 0)
		{
			d_param = (uint16_t)xmltoi(value);
		}
	}

	return true;
}

void	Joint::uninit()
{
	bool ret = false;
	if( enable && torque )
	{
		ret = Device::getInstance()->send(id, TORQUEMODE, 1, DEVICE_DISABLE);
		if( ret == false )
			Logger::getInstance()->log("%d : torque set error!! \n", id);
	}
}

bool	Joint::reset()
{
	// 초기화 에러 유무 검사
	// 임계각 같은 초기 설정 값 일치하는가 검사
	// 기본 포지션을 이동
	uint16_t param = 0;
	bool ret = false;
	
	// enable torque mode
	if( torque )
	{
		ret = Device::getInstance()->recv(id, TORQUEMODE, 1, param);
		if( param != 1)
		{
			ret = Device::getInstance()->send(id, TORQUEMODE, 1, DEVICE_ENABLE);
			if( ret == false )
			{
				Logger::getInstance()->log("%d : torque set error!! \n", id);
				return false;
			}
		}
	}

	// check and adjust cwlimit
	ret = Device::getInstance()->recv(id, CW_LIMIT_ANGLE, 2, param);
	if( ret == false )
	{
		Logger::getInstance()->log("%d : cwlimit recv error!! \n", id);
		return false;
	}
	if(DXL2DEGREE(param) != cwlimit)
	{
		Logger::getInstance()->log("%d : cwlimit is not match : cwlimit : %d, recv cwlimit : %d\n", id, cwlimit, DXL2DEGREE(param));

		ret = Device::getInstance()->send(id, CW_LIMIT_ANGLE, 2, DEGREE2DXL(cwlimit));
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
	ret = Device::getInstance()->recv(id, CCW_LIMIT_ANGLE, 2, param);
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

	// Init PID param
	// P
	param = 0;
	ret = Device::getInstance()->recv(id, P_PARAM, 1, param);
	if (ret == false)
	{
		Logger::getInstance()->log("%d : p-param recv error!! \n", id);
		return false;
	}
	if (p_param >= 0)
	{
		// 설정값과 달라 셋!
		if (param != p_param)
		{
			Logger::getInstance()->log("%d : p_param is not match : p_param : %d, recv p_param : %d\n", id, p_param, param);
			ret = Device::getInstance()->send(id, P_PARAM, 1, (uint16_t)p_param);
			if (ret == true)
			{
				Logger::getInstance()->log("%d : adjust p_param to %d \n", id, p_param);
			}
			else if (ret == false)
			{
				Logger::getInstance()->log("%d : set p_param send error!! \n", id);
				return false;
			}
		}
		else
		{
			Logger::getInstance()->log("%d : PID(p) is good!\n", id);
		}
	}

	// I
	param = 0;
	ret = Device::getInstance()->recv(id, I_PARAM, 1, param);
	if (ret == false)
	{
		Logger::getInstance()->log("%d : i-param recv error!! \n", id);
		return false;
	}
	if (i_param >= 0)
	{
		// 설정값과 달라 셋!
		if (param != i_param)
		{
			Logger::getInstance()->log("%d : i_param is not match : i_param : %d, recv i_param : %d\n", id, i_param, param);
			ret = Device::getInstance()->send(id, I_PARAM, 1, (uint16_t)i_param);
			if (ret == true)
			{
				Logger::getInstance()->log("%d : adjust i_param to %d \n", id, i_param);
			}
			else if (ret == false)
			{
				Logger::getInstance()->log("%d : set i_param send error!! \n", id);
				return false;
			}
		}
		else
		{
			Logger::getInstance()->log("%d : PID(i) is good!\n", id);
		}
	}

	// D
	param = 0;
	ret = Device::getInstance()->recv(id, D_PARAM, 1, param);
	if (ret == false)
	{
		Logger::getInstance()->log("%d : d-param recv error!! \n", id);
		return false;
	}
	if (d_param >= 0)
	{
		// 설정값과 달라 셋!
		if (param != d_param)
		{
			Logger::getInstance()->log("%d : p_param is not match : d_param : %d, recv d_param : %d\n", id, d_param, param);

			ret = Device::getInstance()->send(id, D_PARAM, 1, (uint16_t)d_param);
			if (ret == true)
			{
				Logger::getInstance()->log("%d : adjust d_param to %d \n", id, d_param);
			}
			else if (ret == false)
			{
				Logger::getInstance()->log("%d : set d_param send error!! \n", id);
				return false;
			}
		}
		else
		{
			Logger::getInstance()->log("%d : PID(d) is good!\n", id);
		}
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
	ret = Device::getInstance()->sendqueue();
	enable = true;

	return true;
}

/*
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
	return Device::getInstance()->recv(id, command, 2, param);
}
*/

int		Joint::trimangle(int angle)
{
	if( angle < cwlimit )
		angle = cwlimit;
	if( angle > ccwlimit )
		angle = ccwlimit;
	return angle;
}
