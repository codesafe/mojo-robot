#include "display.h"
#include "device.h"
#include "e-ink.h"

Display::Display()
{

}
Display::~Display()
{

}

bool	Display::init(XMLNode node)
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
	}

	return true;
}

void	Display::uninit()
{

}

bool	Display::reset()
{
	// TODO. 초기화 에러 유무 검사
	// 1. handshake 해본다
	int ret = Device::getInstance()->sendcommand(CMD_HANDSHAKE);

	uint8_t storage[1] = { MEM_TF };
	ret = Device::getInstance()->sendcommand(CMD_MEMORYMODE, storage, 1);

	if( ret == COMM_SUCCESS )
	{
		uint8_t name[] = {"PIC4.BMP" };
		ret = Device::getInstance()->sendcommand(CMD_DRAW_BITMAP, name, strlen((const char*)name));

		if( ret == COMM_SUCCESS )
			ret = Device::getInstance()->sendcommand(CMD_UPDATE);
	}

	return ret == COMM_SUCCESS ? true : false;
}

bool	Display::sendcommand(uint16_t command, uint16_t param)
{

	return true;
}


bool	Display::recvcommand(uint16_t command, uint16_t &param)
{

	return true;
}