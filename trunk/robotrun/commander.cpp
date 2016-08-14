#include "commander.h"
#include "animation.h"


Commander *	Commander::instance = NULL;


Commander::Commander()
{

}

Commander::~Commander()
{

}

void	Commander::addcommand(int type, char command, char *data, int datalen)
{
	Command com;
	com.type = type;
	com.command = command;
	memcpy(com.data, data, SOCKET_BUFFER);
	commandlist.push_back(com);
}

void	Commander::update()
{
	for(size_t i=0; i<commandlist.size(); i++)
	{
		if( commandlist[i].type == COMMAND_DEVICE )
		{
			// TODO. 바로 처리
		}
	}

	for(size_t i=0; i<commandlist.size(); i++)
	{
		if( commandlist[i].type == COMMAND_WHEEL )
		{
			// TODO. 바로 처리
		}
	}

	for(size_t i=0; i<commandlist.size(); i++)
	{
		if( commandlist[i].type == COMMAND_DISPLAY )
		{
			// TODO. 바로 처리
		}
	}
	
	// 에니메이션 처리
	std::deque<Command>::iterator it = commandlist.begin();
	for(; it != commandlist.end(); it++)
	{
		if( it->type == COMMAND_ANIMATION )
		{
			// 현재 플레이 중인 에니메이션이 없는 경우에만 대기중인 명령 실행
			if(Animation::getInstance()->isplaying() == false)
			{
				Animation::getInstance()->play(it->data);
				commandlist.erase(it);
				break;
			}
		}
	}
	
}