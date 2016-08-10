#include "wheel.h"
#include "device.h"

Wheel::Wheel()
{

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
			Logger::getInstance()->log("%d : torque set error!! \n", id);
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
			Logger::getInstance()->log("%d : torque set error!! \n", id);
			return false;
		}
	}

	ret = Device::getInstance()->addsendqueue(id, MOVE_SPEED, 0);
	enable = true;
	return true;
}