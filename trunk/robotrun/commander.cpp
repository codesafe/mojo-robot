﻿#include "commander.h"
#include "animation.h"
#include "part.h"

Commander *	Commander::instance = NULL;


Commander::Commander()
{

}

Commander::~Commander()
{

}

void	Commander::addcommand(int type, char packet, char *data, int datalen)
{
	Command com;
	com.type = type;
	com.packet = packet;
	memcpy(com.data, data, datalen);
	commandlist.push_back(com);
}

void	Commander::update()
{
	std::deque<Command>::iterator it;

	for(size_t i=0; i<commandlist.size(); i++)
	{
		if( commandlist[i].type == COMMAND_DEVICE )
		{
			// TODO. 바로 처리
		}
	}

	for(it = commandlist.begin(); it != commandlist.end();)
	{
		if( it->type == COMMAND_WHEEL )
		{
			// TODO. 바로 처리
			bool ret = false;
			switch(it->packet)
			{
			case WHEEL_FORWARD :
				{
					int lwheelid = PartController::getInstance()->getid("leftwheel");
					int rwheelid = PartController::getInstance()->getid("rightwheel");

					int p = (int&)*(it->data);
					uint16_t param = p;
					ret = PartController::getInstance()->addsendqueuecommand(lwheelid, MOVE_SPEED, param);
					ret = PartController::getInstance()->addsendqueuecommand(rwheelid, MOVE_SPEED, REVERSEWHEEL(param));
					ret = PartController::getInstance()->sendsendqueuecommand();
				}
				break;
			case WHEEL_BACKWARD :
				{
					int lwheelid = PartController::getInstance()->getid("leftwheel");
					int rwheelid = PartController::getInstance()->getid("rightwheel");
					int p = (int&)*(it->data);
					uint16_t param = p;
					ret = PartController::getInstance()->addsendqueuecommand(lwheelid, MOVE_SPEED, REVERSEWHEEL(param));
					ret = PartController::getInstance()->addsendqueuecommand(rwheelid, MOVE_SPEED, param);
					ret = PartController::getInstance()->sendsendqueuecommand();
				}
				break;

			case WHEEL_TURNLEFT :
				{
					int lwheelid = PartController::getInstance()->getid("leftwheel");
					int rwheelid = PartController::getInstance()->getid("rightwheel");
					
					int lp = (int&)*(it->data);
					int rp = (int&)*(it->data+sizeof(int));

					uint16_t lparam = lp;
					uint16_t rparam = rp;

					ret = PartController::getInstance()->addsendqueuecommand(lwheelid, MOVE_SPEED, lparam);
					ret = PartController::getInstance()->addsendqueuecommand(rwheelid, MOVE_SPEED, rparam);
					ret = PartController::getInstance()->sendsendqueuecommand();
				}
				break;

			case WHEEL_TURNRIGHT :
				{
					int lwheelid = PartController::getInstance()->getid("leftwheel");
					int rwheelid = PartController::getInstance()->getid("rightwheel");

					int lp = (int&)*(it->data);
					int rp = (int&)*(it->data+sizeof(int));

					uint16_t lparam = lp;
					uint16_t rparam = rp;

					ret = PartController::getInstance()->addsendqueuecommand(lwheelid, MOVE_SPEED, lparam);
					ret = PartController::getInstance()->addsendqueuecommand(rwheelid, MOVE_SPEED, rparam);
					ret = PartController::getInstance()->sendsendqueuecommand();
				}
				break;

			case WHEEL_STOP :
				{
					int lwheelid = PartController::getInstance()->getid("leftwheel");
					int rwheelid = PartController::getInstance()->getid("rightwheel");
					ret = PartController::getInstance()->addsendqueuecommand(lwheelid, MOVE_SPEED, REVERSEWHEEL(0));
					ret = PartController::getInstance()->addsendqueuecommand(rwheelid, MOVE_SPEED, 0);
					ret = PartController::getInstance()->sendsendqueuecommand();
				}
				break;
			}

			it = commandlist.erase(it);
			continue;
		}

		it++;
	}

	for(size_t i=0; i<commandlist.size(); i++)
	{
		if( commandlist[i].type == COMMAND_DISPLAY )
		{
			// TODO. 바로 처리
		}
	}
	
	// 에니메이션 처리
	for(it = commandlist.begin(); it != commandlist.end(); it++)
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