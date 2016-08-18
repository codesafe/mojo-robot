#include "predef.h"
#include "device.h"
#include "part.h"
#include "timer.h"
#include "animation.h"
#include "network.h"
#include "commander.h"
#include "patch.h"

#ifdef _WIN32

#include "pthread.h"
//#include <curl.h>

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
	Device::getInstance()->sendqueue();
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
		Logger::getInstance()->log(LOG_INFO, "log %s\n", str.c_str());
	}

	return Device::getInstance()->init();
}

void loadanimation()
{
	Animation::getInstance()->load("animation.xml");
	Logger::getInstance()->log(LOG_INFO, "Animation data loaded...\n");
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
		Logger::getInstance()->log(LOG_ERR, "winsock init error!!\n");
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
#if WIN32
	::Sleep(0);
#else
	::sleep(0);
#endif

	Network::getinstance()->update();
	Animation::getInstance()->update();
	Commander::getinstance()->update();
}

//////////////////////////////////////////////////////////////////////////

#if 0

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}


int main()
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	char *url = "http://localhost:8000/aaa.txt";
	char outfilename[FILENAME_MAX] = "d:\\bbb.txt";
	curl = curl_easy_init();
	if (curl) 
	{
		fp = fopen(outfilename,"wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
	}

	return 0;
}

#else

int main()
{
	onlinepatchrobotdata();
//	initthread();
	bool ret = loadsetup();
	if (ret == false)
	{
		Logger::getInstance()->log(LOG_ERR, "Fatal Error.. Load setup faild...\n");
		return -1;
	}

	if (initapplication() == false)
		return -1;
		
	while (1)
	{
			mainupdate();
//			double t = Timer::getInstance()->getCurrentTime();
//			printf("Time %f : \n", t);
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

#endif