#include "part.h"
#include "joint.h"
#include "wheel.h"
#include "display.h"
#include "device.h"

Part::Part()
{
	enable = false;
}

Part::~Part()
{
}

bool	Part::init(XMLNode node)
{
	const char *typestr = node.getAttribute("type");

	if( strcmp(typestr, "display") == 0 )
		type = PART_TYPE_DISPLAY;
	else if( strcmp(typestr, "joint") == 0 )
		type = PART_TYPE_JOINT;
	else if( strcmp(typestr, "wheel") == 0 )
		type = PART_TYPE_WHEEL;
	else
	{
		Logger::getInstance()->log("Unknown Part type %s\n", typestr);
		return false;
	}
	name = node.getName();

	return true;
}

void	Part::uninit()
{

}

bool Part::reset()
{
	enable = true;
	return true;
}


//////////////////////////////////////////////////////////////////////////

PartController * PartController::instance = NULL;

PartController::PartController()
{
}

PartController::~PartController()
{
}

void	PartController::init(XMLNode node)
{
	for (int i = 0; i < node.nChildNode(); i++)
	{
		XMLNode child = node.getChildNode(i);
		const char *typestr = child.getAttribute("type");
		if( strcmp(typestr, "display") == 0 )
		{
			Display *disp = new Display();
			bool ret = disp->init(child);
			if( ret )
			{
				if( disp->reset() )
					partlist.insert(std::make_pair(disp->getid(), disp));
			}

		}
		else if( strcmp(typestr, "joint") == 0 )
		{
			Joint *joint = new Joint();
			bool ret = joint->init(child);
			if( ret )
			{
				if( joint->reset() )
					partlist.insert(std::make_pair(joint->getid(), joint));
			}
		}
		else if( strcmp(typestr, "wheel") == 0 )
		{
			Wheel *wheel = new Wheel();
			bool ret = wheel->init(child);
			if( ret )
			{
				if( wheel->reset() )
					partlist.insert(std::make_pair(wheel->getid(), wheel));
			}
		}
		else
		{
			Logger::getInstance()->log("Unknown Part type %s\n", typestr);
		}
	}
}

void	PartController::uninit()
{
	std::map<int, Part*>::iterator it = partlist.begin();
	for(; it != partlist.end(); it++)
	{
		it->second->uninit();
		delete it->second;
	}

	partlist.clear();
}

bool	PartController::addsendqueuecommand(uint8_t id, uint16_t command, uint16_t param)
{
	bool ret = false;
	std::map<int, Part*>::iterator it = partlist.find(id);
	if (it != partlist.end())
	{
		it->second->addsendcommand(command, param);
		ret = true; 
	}
	else
	{
		Logger::getInstance()->log("Not found parts : %d\n", id);
	}

	return ret;
}

bool	PartController::sendsendqueuecommand()
{
	return Device::getInstance()->sendqueue();
}

bool	PartController::sendcommand(int id, uint16_t command , uint16_t param)
{
	bool ret = false;
	std::map<int, Part*>::iterator it = partlist.find(id);
	if( it != partlist.end() )
	{
		ret = it->second->sendcommand(command , param);
	}
	else
	{
		Logger::getInstance()->log("Not found parts : %d\n", id);
	}
	return ret;
}

bool	PartController::recvcommand(int id, uint16_t command, uint16_t &param)
{
	bool ret = false;
	std::map<int, Part*>::iterator it = partlist.find(id);
	if( it != partlist.end() )
	{
		ret = it->second->recvcommand(command, param);
	}
	else
	{
		Logger::getInstance()->log("Not found parts : %d\n", id);
	}
	return ret;
}


Part*	PartController::getpart(int type, int id)
{
	std::map<int, Part*>::iterator it = partlist.find(id);
	if( it != partlist.end() )
	{
		if( it->second->gettype() == type )
			return it->second;
	}
	return NULL;
}