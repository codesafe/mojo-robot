#include "wheel.h"
#include "device.h"

Wheel::Wheel()
{
	torquelimit = -1;
	p_param = -1;
	i_param = -1;
	d_param = -1;
}


Wheel::~Wheel()
{

}

bool	Wheel::init(XMLNode node)
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
		else if(strcmp(name, "torque") == 0)
		{
			torque = (xmltoi(value) == 1 ? true : false);
		}
		else if(strcmp(name, "torquelimit") == 0)
		{
			torquelimit = xmltoi(value);
		}
#ifdef MX_28
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
#endif

	}

	return true;
}

void	Wheel::uninit()
{
	bool ret = false;
	if( torque && enable)
	{
		ret = Device::getInstance()->send(id, TORQUEMODE, 1, DEVICE_DISABLE);
		if( ret == false )
			Logger::log(LOG_ERR, "%d : torque set error!! \n", id);
	}
}

bool	Wheel::reset()
{
	// TODO. 초기화 에러 유무 검사
	uint16_t param = 0;
	bool ret = false;
	
	// enable torque mode
	if( torque )
	{
		ret = Device::getInstance()->send(id, TORQUEMODE, 1, DEVICE_ENABLE);
		if( ret == false )
		{
			Logger::log(LOG_ERR, "%d : torque set error!! \n", id);
			return false;
		}
	}

	// Torque limit
	if(torquelimit > -1)
	{
		ret = Device::getInstance()->recv(id, TORQUELIMIT, 2, param);
		if( ret == false )
		{
			Logger::log(LOG_ERR, "%d : torque limit recv error!! \n", id);
			return false;
		}

		if( torquelimit != param )
		{
			Logger::log(LOG_WARN, "%d : torquelimit is not match : torquelimit : %d, recv torquelimit : %d\n", id, torquelimit, param);

			ret = Device::getInstance()->send(id, TORQUELIMIT, 2, torquelimit);
			if( ret == true )
			{
				Logger::log(LOG_INFO, "%d : adjust torquelimit to %d \n", id, torquelimit);
			}
			else if( ret == false )
			{
				Logger::log(LOG_ERR, "%d : set torquelimit send error!! \n", id);
				return false;
			}
		}
	}


	// Init PID param

	// P
	param = 0;
	ret = Device::getInstance()->recv(id, P_PARAM, 1, param);
	if (ret == false)
	{
		Logger::log(LOG_ERR, "%d : p-param recv error!! \n", id);
		return false;
	}
	if (p_param >= 0)
	{
		// 설정값과 달라 셋!
		if (param != p_param)
		{
			Logger::log(LOG_WARN, "%d : p_param is not match : p_param : %d, recv p_param : %d\n", id, p_param, param);
			ret = Device::getInstance()->send(id, P_PARAM, 1, (uint16_t)p_param);
			if (ret == true)
			{
				Logger::log(LOG_INFO, "%d : adjust p_param to %d \n", id, p_param);
			}
			else if (ret == false)
			{
				Logger::log(LOG_ERR, "%d : set p_param send error!! \n", id);
				return false;
			}
		}
		else
		{
			Logger::log(LOG_INFO, "%d : PID(p) is good!\n", id);
		}
	}

	// I
	param = 0;
	ret = Device::getInstance()->recv(id, I_PARAM, 1, param);
	if (ret == false)
	{
		Logger::log(LOG_ERR, "%d : i-param recv error!! \n", id);
		return false;
	}
	if (i_param >= 0)
	{
		// 설정값과 달라 셋!
		if (param != i_param)
		{
			Logger::log(LOG_WARN, "%d : i_param is not match : i_param : %d, recv i_param : %d\n", id, i_param, param);
			ret = Device::getInstance()->send(id, I_PARAM, 1, (uint16_t)i_param);
			if (ret == true)
			{
				Logger::log(LOG_INFO, "%d : adjust i_param to %d \n", id, i_param);
			}
			else if (ret == false)
			{
				Logger::log(LOG_ERR, "%d : set i_param send error!! \n", id);
				return false;
			}
		}
		else
		{
			Logger::log(LOG_INFO, "%d : PID(i) is good!\n", id);
		}
	}

	// D
	param = 0;
	ret = Device::getInstance()->recv(id, D_PARAM, 1, param);
	if (ret == false)
	{
		Logger::log(LOG_ERR, "%d : d-param recv error!! \n", id);
		return false;
	}
	if (d_param >= 0)
	{
		// 설정값과 달라 셋!
		if (param != d_param)
		{
			Logger::log(LOG_WARN, "%d : p_param is not match : d_param : %d, recv d_param : %d\n", id, d_param, param);

			ret = Device::getInstance()->send(id, D_PARAM, 1, (uint16_t)d_param);
			if (ret == true)
			{
				Logger::log(LOG_INFO, "%d : adjust d_param to %d \n", id, d_param);
			}
			else if (ret == false)
			{
				Logger::log(LOG_ERR, "%d : set d_param send error!! \n", id);
				return false;
			}
		}
		else
		{
			Logger::log(LOG_INFO, "%d : PID(d) is good!\n", id);
		}
	}


	// 정지
	ret = Device::getInstance()->addsendqueue(id, MOVE_SPEED, 0);
	enable = true;
	return true;
}