#include "predef.h"
#include "device.h"
#include "part.h"
#include "timer.h"
#include "animation.h"
#include "network.h"
#include "commander.h"
#include "patch.h"
#include "utils.h"

#ifdef _WIN32
#include "pthread.h"
#else
#include <curl/curl.h>
#endif


//////////////////////////////////////////////////////////////////////////

// 온라인으로 패치
bool onlinepatchrobotdata()
{
	Patch patch;
	patch.dopatch();
	return true;
}

//////////////////////////////////////////////////////////////////////////

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
	//Device::getInstance()->sendqueue();
}


bool loadglobalinfo(XMLNode pnode)
{
	MemDB::getInstance()->reset();

	for (int i = 0; i < pnode.nChildNode(); i++)
	{
		XMLNode node = pnode.getChildNode(i);
		const char *name = node.getName();
		const char *value = node.getText();
		MemDB::getInstance()->setValue(name, value);

		std::string str = "name : " + std::string(name) + " value : " + value;
		Logger::log(LOG_INFO, "log %s\n", str.c_str());
	}

	return Device::getInstance()->init();
}

void loadanimation()
{
	Animation::getInstance()->load("animation.xml", "display.xml");
	Logger::log(LOG_INFO, "Animation data loaded...\n");
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

	loadanimation();

	return true;
}

void releaseall()
{
	PartController::getInstance()->uninit();
	Device::getInstance()->uninit();			// close all device and ports
}


bool opennetwork()
{
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		Logger::log(LOG_ERR, "winsock init error!!\n");
		return false;
	}
#endif
	Network::getinstance()->init();
	Network::getinstance()->connect();

	return true;
}

void closenetwork()
{
	Network::getinstance()->uninit();
#ifdef WIN32
	WSACleanup();
#endif
}

//////////////////////////////////////////////////////////////////////////

void restartapp()
{
#ifdef WIN32
	//WinExec("./restart.bat", SW_SHOW);
	ShellExecute(NULL, "open", ".\\restart.bat", NULL, NULL, SW_SHOW);
#else
	system("./restart.sh");
#endif
}


void mainupdate()
{
	while (true)
	{
#if WIN32
		::Sleep(10);
#else
		::sleep(10);
#endif
		Network::getinstance()->update();
		Animation::getInstance()->update();
		int ret = Commander::getinstance()->update();
		if (ret != 0)
		{
			if (ret == DEVICERESET)
			{
				restartapp();
				break;		
			}
			else if (ret == FORCEPATCH)
			{

			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv)
{
	if( argc > 1 ) 
	{
		printf("Server IP is %s\n", argv[1]);
		MemDB::getInstance()->setValue("serveraddress", argv[1]);
	}

	opennetwork();
	onlinepatchrobotdata();

	if (loadsetup() == false)
	{
		Logger::log(LOG_ERR, "Fatal Error.. Load setup faild...\n");
		return -1;
	}
		
	mainupdate();

	closenetwork();
	return 0;
}

