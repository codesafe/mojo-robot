﻿
#include "animation.h"
#include "timer.h"
#include "device.h"
#include "part.h"
#include "joint.h"

Motion::Motion()
{
	totaltime = 0;
	state = STATE_IDLE;
}

Motion::~Motion()
{
}

bool	Motion::init(XMLNode pnode)
{
	float maxtime = 0.f;
	name = pnode.getAttribute("name");

	for (int p = 0; p < pnode.nChildNode(); p++)
	{
		XMLNode node = pnode.getChildNode(p);

		const char *alias = node.getAttribute("alias");
		const char *idstr = node.getAttribute("deviceid");
		int deviceid = xmltoi(idstr);

		std::deque<_MOTION>	motions;
		// 모션들
		for (int i = 0; i < node.nChildNode(); i++)
		{
			// parts
			_MOTION _motion;

			XMLNode chnode = node.getChildNode(i);
			for (int ch = 0; ch < chnode.nChildNode(); ch++)
			{
				XMLNode _node = chnode.getChildNode(ch);
				const char *name = _node.getName();
				const char *value = _node.getText();
				if(std::string("starttime") == name)
				{
					_motion.starttime = (float)xmltof(value);
				}
				else if(std::string("endtime") == name)
				{
					_motion.endtime = (float)xmltof(value);
					if(maxtime < _motion.endtime)
						maxtime = _motion.endtime;
				}
				else if(std::string("angle") == name)
				{
					_motion.angle = xmltoi(value);
				}
			}

			motions.push_back(_motion);
		}

		// motion for part
		motionlist.insert(std::make_pair(deviceid, motions));
	}

	totaltime = maxtime;
	return true;
}

void	Motion::uninit()
{
	motionlist.clear();
}

std::string	Motion::getname()
{
	return name;
}

// update !!
bool	Motion::update()
{
	bool ret = true;
	if( state == STATE_IDLE || state == STATE_STOPPED )
	{
		if(state == STATE_STOPPED) setstate(STATE_IDLE);
		ret = false;
		return ret;
	}

	// 업데이트
	bool needsend = false;
	double ctime = Timer::getInstance()->getCurrentTime();
	updatetime = (float)(ctime - starttime)/(float)CLOCKS_PER_SEC;

	std::map< int, std::deque<_MOTION> >::iterator it = currentmotion.begin();
	for(; it != currentmotion.end(); it++)
	{
		if( !it->second.empty() )
		{
			if( it->second[0].starttime <= updatetime )
			{
				if( it->second[0].endtime > updatetime )
				{
					// 해당 에니메이션 시작
					int id = it->first;
					int angle = it->second[0].angle;
					float speed = it->second[0].endtime - it->second[0].starttime;
					bool ret = _play(id, angle, speed);
					if(ret)
					{
						needsend |= true;
						it->second.pop_front();
						printf("change motion time -> %f\n", updatetime);
					}
				}
				else
				{
					// 완전히 넘어감.. 혹은 끝남 --> skip
					it->second.pop_front();
				}
			}
		}
	}

	if(needsend)
		PartController::getInstance()->sendsendqueuecommand();

	if(totaltime <= updatetime)
	{
		setstate(STATE_STOPPED);
		ret = false;
	}

	printf("time -> %f\n", updatetime);
	return ret;
}

// 모션 플레이
bool	Motion::play()
{
	bool ret = false;
	currentmotion.clear();

	std::map< int, std::deque<_MOTION> >::iterator oit = motionlist.begin();
	// make clone
	for ( ; oit != motionlist.end();oit++)
		currentmotion.insert(std::make_pair(oit->first, oit->second));

	// 0 프레임부터 시작!
	std::map< int, std::deque<_MOTION> >::iterator it = currentmotion.begin();
	for ( ; it != currentmotion.end(); it++)
	{
		int id = it->first;
		int angle = it->second[0].angle;
		float speed = it->second[0].endtime - it->second[0].starttime;

		// ccw / cw 제한각 안쪽으로 trim
		Part *part = PartController::getInstance()->getpart(PART_TYPE_JOINT, id);
		if( part != NULL )
		{
			Joint *joint = (Joint*)part;
			angle = joint->trimangle(angle);
		}

		ret = _play(id, angle, speed);
		if( ret )
			it->second.pop_front();
	}

	ret = PartController::getInstance()->sendsendqueuecommand();
	setstate(STATE_RUNNING);
	updatetime = 0;
	starttime = Timer::getInstance()->getCurrentTime();

	return ret;
}


bool	Motion::_play(int id, int angle, float speed)
{
	bool ret = false;
	uint16_t currentangle = 0;
#if 1

	if (PartController::getInstance()->recvcommand(id, CURRENT_POSITION, currentangle) == true)
	{
		currentangle = DXL2DEGREE(currentangle);
		int destangle = abs(currentangle - angle);

		uint16_t p = SPEEDVALUE(speed, destangle);
		ret = PartController::getInstance()->addsendqueuecommand(id, MOVE_SPEED, p);
		ret = PartController::getInstance()->addsendqueuecommand(id, DEST_POSITION, DEGREE2DXL(angle));


		Logger::getInstance()->log("change motion! \n");
	}

#else

	if( Device::getInstance()->recv(id, CURRENT_POSITION, currentangle) == true )
	{
		currentangle = DXL2DEGREE(currentangle);
		int destangle = abs(currentangle - angle);

		uint16_t p = SPEEDVALUE(speed, destangle);
		ret = Device::getInstance()->addsendqueue(id, MOVE_SPEED, p);
		ret = Device::getInstance()->addsendqueue(id, DEST_POSITION, DEGREE2DXL(angle));
	}
#endif
	return ret;
}

// 에니메이션 정지!!
void	Motion::stop()
{
	currentmotion.clear();
	setstate(STATE_STOPPED);
}


//////////////////////////////////////////////////////////////////////////

Animation * Animation::instance = NULL;

Animation::Animation() : currentmotion(NULL)
{
}

Animation::~Animation()
{
}

bool	Animation::init()
{

	return true;
}

void	Animation::uninit()
{
	std::map<std::string, Motion*>::iterator it = animationlist.begin();
	for (; it != animationlist.end(); it++)
		delete it->second;

	animationlist.clear();
	animationfilelist.clear();
	currentmotion = NULL;
}

// 에니메이션 완료 추적 / 다음 part연결
void	Animation::update()
{
	if( currentmotion != NULL )
	{
		bool ret = currentmotion->update();
		if(ret == false)
			currentmotion = NULL;
	}
}

// 에니메이션 로딩 / parse
void	Animation::load(std::string filename)
{
	uninit();

	XMLNode root = XMLNode::openFileHelper(filename.c_str(), "");
	for (int i = 0; i < root.nChildNode(); i++)
	{
		XMLNode aninode = root.getChildNode(i);
		const char *version = aninode.getAttribute("version");
		const char *filename = aninode.getAttribute("filename");

		ANIFILE anifile;
		anifile.version = atoi(version);
		anifile.filename = filename;
		animationfilelist.push_back(anifile);
	}

	//////////////////////////////////////////////////////////////////////////

	for(size_t i=0; i<animationfilelist.size(); i++)
	{
		XMLNode root = XMLNode::openFileHelper(animationfilelist[i].filename.c_str(), "");
		for (int i = 0; i < root.nChildNode(); i++)
		{
			XMLNode aninode = root.getChildNode(i);
			const char *name = aninode.getAttribute("name");

			Motion *motion = new Motion();
			motion->init(aninode);
			animationlist.insert(std::make_pair(name, motion));
		}
	}


}

// 에니메이션 시작 (joint + display)
void	Animation::play(std::string name)
{
	if( isplaying()	== false )
	{
		std::map<std::string, Motion*>::iterator it = animationlist.find(name);
		if(it != animationlist.end())
		{
			it->second->play();
			currentmotion = it->second;
		}
		else
		{
			Logger::getInstance()->log("Not found animation %s\n", name.c_str());
		}
	}
}

// 정지!! forcestop 
void	Animation::stop()
{
	if( currentmotion != NULL)
	{
		currentmotion->stop();
		currentmotion = NULL;
	}
}

bool	Animation::isplaying()
{
	// 모션들이 멈추었나??
	if( currentmotion != NULL )
		return currentmotion->getstate() == STATE_RUNNING ? true : false;

	return false;
}
