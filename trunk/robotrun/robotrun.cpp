#include "predef.h"
#include "device.h"
#include "part.h"
#include "timer.h"
#include "animation.h"
#include "network.h"
#include "commander.h"

#ifdef _WIN32
#include "pthread.h"
#else

#endif


int getch()
{
#ifdef __linux__
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
#elif defined(_WIN32) || defined(_WIN64)
	return _getch();
#endif
}

// 파트 초기화
void setupparts(XMLNode node)
{
	PartController::getInstance()->init(node);
	Device::getInstance()->sendqueue();
}


bool loadglobalinfo(XMLNode pnode)
{
	for (int i = 0; i < pnode.nChildNode(); i++)
	{
		XMLNode node = pnode.getChildNode(i);


		const char *name = node.getName();
		const char *value = node.getText();
		MemDB::getInstance()->setValue(name, value);

		std::string str = "name : " + std::string(name) + " value : " + value;
		Logger::getInstance()->log("log %s\n", str.c_str());
	}

	return Device::getInstance()->init();
}

// 설정 로딩
bool loadsetup()
{
	XMLNode root = XMLNode::openFileHelper("setup.xml", "");
	for (int i = 0; i < root.nChildNode(); i++)
	{
		XMLNode node = root.getChildNode(i);
		const char *name = node.getName();

		if (std::string(name) == "global")
		{
			bool ret = loadglobalinfo(node);
			if(ret == false)
				return false;
		}
		else if (std::string(name) == "parts")
		{
			setupparts(node);
		}
	}

	return true;
}

void loadanimation()
{
	Animation::getInstance()->load("animation.xml");
}


void releaseall()
{
	PartController::getInstance()->uninit();
	Device::getInstance()->uninit();			// close all device and ports
	Network::getinstance()->uninit();

#ifdef WIN32
	WSACleanup();
#endif
}


bool initapplication()
{
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		Logger::getInstance()->log("winsock init error!!\n");
		return false;
	}
#endif
	Network::getinstance()->init();
	Network::getinstance()->connect();

	return true;
}


void * thr_fn(void *arg)
{
	printf("create new thread\n");
	return((void *)0);
}

void initthread()
{
	pthread_t tid;
	int err;
	err = pthread_create(&tid, NULL, thr_fn, NULL);
}

//////////////////////////////////////////////////////////////////////////

void mainupdate()
{
	Network::getinstance()->update();
	Network::getinstance()->read();
	Animation::getInstance()->update();

	Commander::getinstance()->update();
}

//////////////////////////////////////////////////////////////////////////

int main()
{
//	initthread();
	bool ret = loadsetup();

	if (initapplication() == false)
		return -1;

	loadanimation();
		
//		Animation::getInstance()->play("test");

	while (1)
	{
#ifdef __linux__			
			sleep(0);
#else
			Sleep(0);
#endif
		mainupdate();

//			Animation::getInstance()->update();

// 			clock_t t = Timer::getInstance()->getticktime();
// 			printf("Time %d : \n", t);
// 
// 			float s = Timer::getInstance()->getsecond();
// 			printf("Second %f : \n", s);
	

#ifdef TESTBUILD
// 		if (getch() == ESC_ASCII_VALUE)
// 		{
// 			releaseall();
// 			break;
// 		}
#endif

	}

	return 0;
}
