#include "display.h"
#include "device.h"
#include "e-ink.h"
#include "utils.h"


bool Display::exitthread[EYE_MAX];
bool Display::actionTrigger[EYE_MAX];
pthread_mutex_t Display::mutex_lock[EYE_MAX];
std::vector<DisplayInfo>	Display::displayinfolist[EYE_MAX];

Display::Display()
{
	rotation = 0;
	storage = MEM_NAND;
}

Display::~Display()
{
	pthread_mutex_destroy(&mutex_lock[side]);
}

void * Display::thread_fn(void *arg)
{
	printf("create new thread : %d \n", (int)arg);
	int _side = (int)arg;

	while (true)
	{
		Utils::Sleep(10);

		pthread_mutex_lock(&mutex_lock[_side]);
		if(exitthread[_side])
		{
			pthread_mutex_unlock(&mutex_lock[_side]);
			break;
		}

		if (actionTrigger[_side])
		{
			int ret = COMM_NOT_AVAILABLE;
			for (size_t i = 0; i < displayinfolist[_side].size(); i++)
			{
				ret = Device::getInstance()->sendcommand(displayinfolist[_side][i].side, displayinfolist[_side][i].command, 
					displayinfolist[_side][i].param, displayinfolist[_side][i].length);
				if (ret != COMM_SUCCESS)
					break;
			}

			displayinfolist[_side].clear();
			actionTrigger[_side] = false;
		}

		pthread_mutex_unlock(&mutex_lock[_side]);
	}

	pthread_exit((void *)0);
	return((void *)0);
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
		else if (strcmp(name, "storage") == 0)
		{
			storage = strcmp(value, "flash") == 0 ? MEM_NAND : MEM_TF;
		}
		else if (strcmp(name, "side") == 0)
		{
			side = strcmp(value, "left") == 0 ? LEFT_EYE : RIGHT_EYE;
		}
		else if (strcmp(name, "rotation") == 0)
		{
			rotation = strcmp(value, "true") == 0 ? 1 : 0;
		}
		else if (strcmp(name, "initpic") == 0)
		{
			initpic = value;
		}
	}

	actionTrigger[side] = false;
	exitthread[side] = false;

	pthread_mutex_init(&mutex_lock[side], NULL);
	int err = pthread_create(&threadid, NULL, thread_fn, (void *)side);
	return true;
}

void	Display::uninit()
{
	pthread_mutex_lock(&mutex_lock[side]);
	exitthread[side] = true;
	pthread_mutex_unlock(&mutex_lock[side]);
}

bool	Display::reset()
{
	// 초기화 에러 유무 검사
	// handshake 해본다
	int ret = Device::getInstance()->sendcommand(side, CMD_HANDSHAKE);
	if (ret == COMM_SUCCESS)
	{
		ret = Device::getInstance()->sendcommand(side, CMD_MEMORYMODE, &storage, 1);
		if (ret == COMM_SUCCESS)
		{
			ret = Device::getInstance()->sendcommand(side, CMD_SCREEN_ROTATION, &rotation, 1);
			if (ret == COMM_SUCCESS)
			{
				addcommandlist(CMD_DRAW_BITMAP, (uint8_t*)initpic.c_str(), strlen((const char*)initpic.c_str()));
				addcommandlist(CMD_UPDATE);
				ret = flushcommandlist();
			}
		}
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


void	Display::addcommandlist(uint8_t command, uint8_t *param, int length)
{
	DisplayInfo info;
	info.side = side;
	info.command = command;
	info.length = length;
	if(param != NULL)
		memcpy(info.param, param, length);

	pthread_mutex_lock(&mutex_lock[side]);
	displayinfolist[side].push_back(info);
	pthread_mutex_unlock(&mutex_lock[side]);
}

// 전송
int		Display::flushcommandlist()
{
	pthread_mutex_lock(&mutex_lock[side]);
	actionTrigger[side] = true;
	pthread_mutex_unlock(&mutex_lock[side]);
	return 0;
}

