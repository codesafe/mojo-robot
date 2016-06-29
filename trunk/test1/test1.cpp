/*
* read_write.cpp
*
*  Created on: 2016. 2. 21.
*      Author: leon
*/

//
// *********     Read and Write Example      *********
//
//
// Available DXL model on this example : All models using Protocol 1.0
// This example is tested with a DXL MX-28, and an USB2DYNAMIXEL
// Be sure that DXL MX properties are already set as %% ID : 1 / Baudnum : 1 (Baudrate : 1000000)
//

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "dynamixel_sdk.h"                                  // Uses Dynamixel SDK library

#include "xmlParser.h"
#include "memdb.h"
#include "logger.h"

#include <string>



// Control table address
#define ADDR_MX_TORQUE_ENABLE           24                  // Control table address is different in Dynamixel model
#define ADDR_MX_GOAL_POSITION           30
#define ADDR_MX_SPEED					32
#define ADDR_MX_PRESENT_POSITION        36

// Protocol version
#define PROTOCOL_VERSION                1.0                 // See which protocol version is used in the Dynamixel

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
	for(int i=0; i<root.nChildNode(); i++)
	{
		XMLNode node = root.getChildNode(i);
		const char *name = node.getName();
		const char *value = node.getText();
		MemDB::getInstance()->setValue(name, value);

		std::string str = "name : " + std::string(name)+ " value : " + value + "\n";
		printf(str.c_str());

		Logger::getInstance()->log("log %s", str.c_str());
	}

}

int dxid = 0;

int main()
{
	loadxml();

	// Initialize PortHandler instance
	// Set the port path
	// Get methods and members of PortHandlerLinux or PortHandlerWindows
	dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(MemDB::getInstance()->getValue("devicename").c_str());

	// Initialize PacketHandler instance
	// Set the protocol version
	// Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
	dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

	int index = 0;
	int dxl_comm_result = COMM_TX_FAIL;             // Communication result
	int dxl_goal_position[2] = { DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE };         // Goal position

	uint8_t dxl_error = 0;                          // Dynamixel error
	uint16_t dxl_present_position = 0;              // Present position

													// Open port
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
	if ( portHandler->setBaudRate(MemDB::getInstance()->getIntValue("baudrate")) )
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


	dxid = MemDB::getInstance()->getIntValue("rarm0");



	// Enable DXL Torque
	dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, dxid, ADDR_MX_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);

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

	packetHandler->reboot(portHandler, dxid, &dxl_error);


	while (1)
	{
		printf("Press any key to continue! (or press ESC to quit!)\n");
		if (getch() == ESC_ASCII_VALUE)
			break;

		dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, dxid, ADDR_MX_SPEED, 100, &dxl_error);

		// Write goal position
		dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, dxid, ADDR_MX_GOAL_POSITION, dxl_goal_position[index], &dxl_error);
		if (dxl_comm_result != COMM_SUCCESS)
		{
			packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{
			packetHandler->printRxPacketError(dxl_error);
		}

		do
		{
			// Read present position
			dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, dxid, ADDR_MX_PRESENT_POSITION, &dxl_present_position, &dxl_error);
			if (dxl_comm_result != COMM_SUCCESS)
			{
				packetHandler->printTxRxResult(dxl_comm_result);
			}
			else if (dxl_error != 0)
			{
				packetHandler->printRxPacketError(dxl_error);
			}

			printf("[ID:%03d] GoalPos:%03d  PresPos:%03d\n", dxid, dxl_goal_position[index], dxl_present_position);

		} while ((abs(dxl_goal_position[index] - dxl_present_position) > DXL_MOVING_STATUS_THRESHOLD));

		// Change goal position
		if (index == 0)
		{
			index = 1;
		}
		else
		{
			index = 0;
		}
	}

	// Disable Dynamixel Torque
	dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, dxid, ADDR_MX_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
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
