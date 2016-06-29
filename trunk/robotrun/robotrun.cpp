#include "predef.h"
#include "device.h"
#include "part.h"
#include "timer.h"
#include "animation.h"
#include "network.h"

#ifdef _WIN32
#include "pthread.h"
#else

#endif


#if 0

// Control table address
#define ADDR_MX_TORQUE_ENABLE           24                  // Control table address is different in Dynamixel model
#define ADDR_MX_GOAL_POSITION           30
#define ADDR_MX_SPEED					32
#define ADDR_MX_PRESENT_POSITION        36

// Default setting
#define DXL_ID                          53                   // Dynamixel ID: 1
//#define BAUDRATE                        1000000
#define BAUDRATE                        57142


#define DEVICENAME                      "//./COM11"
//#define DEVICENAME                      "/dev/ttyUSB0"      // Check which port is being used on your controller

// ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
//#define DXL_MINIMUM_POSITION_VALUE      100                 // Dynamixel will rotate between this value
//#define DXL_MAXIMUM_POSITION_VALUE      4000                // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MINIMUM_POSITION_VALUE      2055
#define DXL_MAXIMUM_POSITION_VALUE      3080
#define DXL_MOVING_STATUS_THRESHOLD     10                  // Dynamixel moving status threshold

#define ESC_ASCII_VALUE                 0x1b

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

int kbhit(void)
{
#ifdef __linux__
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
#elif defined(_WIN32) || defined(_WIN64)
	return _kbhit();
#endif
}


void loadxml()
{
	XMLNode root = XMLNode::openFileHelper("setup.xml", "");
	for (int i = 0; i < root.nChildNode(); i++)
	{
		XMLNode node = root.getChildNode(i);
		const char *name = node.getName();
		const char *value = node.getText();
		MemDB::getInstance()->setValue(name, value);

		std::string str = "name : " + std::string(name) + " value : " + value + "\n";
		printf(str.c_str());

		Logger::getInstance()->log("log %s", str.c_str());
	}

}

int dxid = 0;

int main()
{
	dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler("//./COM11");
	dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(1.0);

	int index = 0;
	int dxl_comm_result = COMM_TX_FAIL;             // Communication result
	int dxl_goal_position[2] = { DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE };         // Goal position

	uint8_t dxl_error = 0;                          // Dynamixel error
	uint16_t dxl_present_position = 0;              // Present position

	if (portHandler->openPort())
	{
		printf("Succeeded to open the port!\n");
	}
	else
	{
		printf("Failed to open the port!\n");
		printf("Press any key to terminate...\n");
		getch();
		return 0;
	}

	// Set port baudrate
	if (portHandler->setBaudRate(57142))
	{
		printf("Succeeded to change the baudrate!\n");
	}
	else
	{
		printf("Failed to change the baudrate!\n");
		printf("Press any key to terminate...\n");
		getch();
		return 0;
	}

	packetHandler->reboot(portHandler, 42, &dxl_error);

	// Enable DXL Torque
	dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, 42, TORQUEMODE, TORQUE_ENABLE, &dxl_error);

	if (dxl_comm_result != COMM_SUCCESS)
	{
		packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		packetHandler->printRxPacketError(dxl_error);
	}
	else
	{
		printf("Dynamixel has been successfully connected \n");
	}

//	packetHandler->reboot(portHandler, dxid, &dxl_error);


	while (1)
	{
		printf("Press any key to continue! (or press ESC to quit!)\n");
		if (getch() == ESC_ASCII_VALUE)
			break;

		dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, 42, DEST_POSITION, DEGREE2DXL(90), &dxl_error);
	}

	dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, 42, TORQUEMODE, TORQUE_DISABLE, &dxl_error);
	if (dxl_comm_result != COMM_SUCCESS)
	{
		packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		packetHandler->printRxPacketError(dxl_error);
	}

	// Close port
	portHandler->closePort();

	return 0;
}

#else

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


int main()
{
//	initthread();


	if (initapplication() == false)
		return -1;
	
	bool ret = loadsetup();
	loadanimation();
/*
	if( ret == false )
	{
		if (getch() == ESC_ASCII_VALUE)
			return 0;
	}
	else*/
	{
		printf("Press any key to continue! (or press ESC to quit!)\n");
		if (getch() == ESC_ASCII_VALUE)
		{
			releaseall();
			return 0;
		}

		Animation::getInstance()->play("test");

		while (1)
		{
#ifdef __linux__			
			sleep(0);
#else
			Sleep(0);
#endif
			Animation::getInstance()->update();

// 			clock_t t = Timer::getInstance()->getticktime();
// 			printf("Time %d : \n", t);
// 
// 			float s = Timer::getInstance()->getsecond();
// 			printf("Second %f : \n", s);
	
		}

		releaseall();
	}

	return 0;
}

#endif
