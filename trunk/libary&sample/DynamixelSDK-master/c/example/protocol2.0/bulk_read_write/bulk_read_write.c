/*
* bulk_read_write.c
*
*  Created on: 2016. 5. 16.
*      Author: leon
*/

//
// *********     Bulk Read and Bulk Write Example      *********
//
//
// Available Dynamixel model on this example : All models using Protocol 2.0
// This example is designed for using two Dynamixel PRO 54-200, and an USB2DYNAMIXEL. 
// To use another Dynamixel model, such as X series, see their details in E-Manual(support.robotis.com) and edit below "#define"d variables yourself.
// Be sure that Dynamixel PRO properties are already set as %% ID : 1 and 2 / Baudnum : 3 (Baudrate : 1000000 [1M])
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
#include "DynamixelSDK.h"                                   // Uses Dynamixel SDK library

// Control table address
#define ADDR_PRO_TORQUE_ENABLE          562                 // Control table address is different in Dynamixel model
#define ADDR_PRO_LED_RED                563
#define ADDR_PRO_GOAL_POSITION          596
#define ADDR_PRO_PRESENT_POSITION       611

// Data Byte Length
#define LEN_PRO_LED_RED                 1
#define LEN_PRO_GOAL_POSITION           4
#define LEN_PRO_PRESENT_POSITION        4

// Protocol version
#define PROTOCOL_VERSION                2.0                 // See which protocol version is used in the Dynamixel

// Default setting
#define DXL1_ID                         1                   // Dynamixel#1 ID: 1
#define DXL2_ID                         2                   // Dynamixel#2 ID: 2
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"      // Check which port is being used on your controller
                                                            // ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
#define DXL_MINIMUM_POSITION_VALUE      -150000             // Dynamixel will rotate between this value
#define DXL_MAXIMUM_POSITION_VALUE      150000              // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MOVING_STATUS_THRESHOLD     20                  // Dynamixel moving status threshold

#define ESC_ASCII_VALUE                 0x1b

#ifdef __linux__
int _getch()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int _kbhit(void)
{
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
}
#endif

int main()
{
    // Initialize PortHandler Structs
    // Set the port path
    // Get methods and members of PortHandlerLinux or PortHandlerWindows
    int port_num = PortHandler(DEVICENAME);

    // Initialize PacketHandler Structs 
    PacketHandler();

    // Initialize GroupBulkWrite Struct
    int groupwrite_num = GroupBulkWrite(port_num, PROTOCOL_VERSION);
    //GroupBulkWrite groupBulkWrite(portHandler, packetHandler);

    // Initialize Groupbulkread Struct
    int groupread_num = GroupBulkRead(port_num, PROTOCOL_VERSION);

    int index = 0;
    int dxl_comm_result = COMM_TX_FAIL;             // Communication result
    bool dxl_addparam_result = false;               // AddParam result
    bool dxl_getdata_result = false;                // GetParam result
    int dxl_goal_position[2] = { DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE };         // Goal position

    UINT8_T dxl_error = 0;                          // Dynamixel error
    UINT8_T dxl_led_value[2] = { 0x00, 0xFF };      // Dynamixel LED value for write
    INT32_T dxl1_present_position = 0;              // Present position
    UINT8_T dxl2_led_value_read;                    // Dynamixel LED value for read

    // Open port
    if (OpenPort(port_num))
    {
        printf("Succeeded to open the port!\n");
    }
    else
    {
        printf("Failed to open the port!\n");
        printf("Press any key to terminate...\n");
        _getch();
        return 0;
    }

    // Set port baudrate
    if (SetBaudRate(port_num, BAUDRATE))
    {
        printf("Succeeded to change the baudrate!\n");
    }
    else
    {
        printf("Failed to change the baudrate!\n");
        printf("Press any key to terminate...\n");
        _getch();
        return 0;
    }

    // Enable Dynamixel#1 Torque
    Write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE);
    if ((dxl_comm_result = GetLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
        PrintTxRxResult(PROTOCOL_VERSION, dxl_comm_result);
    else if ((dxl_error = GetLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
        PrintRxPacketError(PROTOCOL_VERSION, dxl_error);
    else
        printf("DXL#%d has been successfully connected \n", DXL1_ID);

    // Enable Dynamixel#2 Torque
    Write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE);
    if ((dxl_comm_result = GetLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
        PrintTxRxResult(PROTOCOL_VERSION, dxl_comm_result);
    else if ((dxl_error = GetLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
        PrintRxPacketError(PROTOCOL_VERSION, dxl_error);
    else
        printf("DXL#%d has been successfully connected \n", DXL2_ID);

    // Add parameter storage for Dynamixel#1 present position
    dxl_addparam_result = GroupBulkRead_AddParam(groupread_num, DXL1_ID, ADDR_PRO_PRESENT_POSITION, LEN_PRO_PRESENT_POSITION);
    if (dxl_addparam_result != true)
    {
        fprintf(stderr, "[ID:%03d] grouBulkRead addparam failed", DXL1_ID);
        return 0;
    }

    // Add parameter storage for Dynamixel#2 LED value
    dxl_addparam_result = GroupBulkRead_AddParam(groupread_num, DXL2_ID, ADDR_PRO_LED_RED, LEN_PRO_LED_RED);
    if (dxl_addparam_result != true)
    {
        fprintf(stderr, "[ID:%03d] grouBulkRead addparam failed", DXL2_ID);
        return 0;
    }

    while (1)
    {
        printf("Press any key to continue! (or press ESC to quit!)\n");
        if (_getch() == ESC_ASCII_VALUE)
            break;

        // Add parameter storage for Dynamixel#1 goal position
        dxl_addparam_result = GroupBulkWrite_AddParam(groupwrite_num, DXL1_ID, ADDR_PRO_GOAL_POSITION, LEN_PRO_GOAL_POSITION, dxl_goal_position[index], 4);
        if (dxl_addparam_result != true)
        {
            fprintf(stderr, "[ID:%03d] groupBulkWrite addparam failed", DXL1_ID);
            return 0;
        }

        // Add parameter storage for Dynamixel#2 LED value
        dxl_addparam_result = GroupBulkWrite_AddParam(groupwrite_num, DXL2_ID, ADDR_PRO_LED_RED, LEN_PRO_LED_RED, dxl_led_value[index], 1);
        if (dxl_addparam_result != true)
        {
            fprintf(stderr, "[ID:%03d] groupBulkWrite addparam failed", DXL2_ID);
            return 0;
        }

        // Bulkwrite goal position and LED value
        GroupBulkWrite_TxPacket(groupwrite_num);
        if ((dxl_comm_result = GetLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
            PrintTxRxResult(PROTOCOL_VERSION, dxl_comm_result);

        // Clear bulkwrite parameter storage
        GroupBulkWrite_ClearParam(groupwrite_num);

        do
        {
            // Bulkread present position and LED status
            GroupBulkRead_TxRxPacket(groupread_num);
            if ((dxl_comm_result = GetLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
                PrintTxRxResult(PROTOCOL_VERSION, dxl_comm_result);

            // Check if groupbulkread data of Dynamixel#1 is available
            dxl_getdata_result = GroupBulkRead_IsAvailable(groupread_num, DXL1_ID, ADDR_PRO_PRESENT_POSITION, LEN_PRO_PRESENT_POSITION);
            if (dxl_getdata_result != true)
            {
                fprintf(stderr, "[ID:%03d] groupBulkRead getdata failed", DXL1_ID);
                return 0;
            }

            // Check if groupbulkread data of Dynamixel#2 is available
            dxl_getdata_result = GroupBulkRead_IsAvailable(groupread_num, DXL2_ID, ADDR_PRO_LED_RED, LEN_PRO_LED_RED);
            if (dxl_getdata_result != true)
            {
                fprintf(stderr, "[ID:%03d] groupBulkRead getdata failed", DXL2_ID);
                return 0;
            }

            // Get present position value
            dxl1_present_position = GroupBulkRead_GetData(groupread_num, DXL1_ID, ADDR_PRO_PRESENT_POSITION, LEN_PRO_PRESENT_POSITION);

            // Get LED value
            dxl2_led_value_read = GroupBulkRead_GetData(groupread_num, DXL2_ID, ADDR_PRO_LED_RED, LEN_PRO_LED_RED);

            printf("[ID:%03d] Present Position : %d \t [ID:%03d] LED Value: %d\n", DXL1_ID, dxl1_present_position, DXL2_ID, dxl2_led_value_read);

        } while (abs(dxl_goal_position[index] - dxl1_present_position) > DXL_MOVING_STATUS_THRESHOLD);

        // Change goal position
        if (index == 0)
            index = 1;
        else
            index = 0;
    }

    // Disable Dynamixel#1 Torque
    Write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE);
    if ((dxl_comm_result = GetLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
        PrintTxRxResult(PROTOCOL_VERSION, dxl_comm_result);
    else if ((dxl_error = GetLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
        PrintRxPacketError(PROTOCOL_VERSION, dxl_error);

    // Disable Dynamixel#2 Torque
    Write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE);
    if ((dxl_comm_result = GetLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
        PrintTxRxResult(PROTOCOL_VERSION, dxl_comm_result);
    else if ((dxl_error = GetLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
        PrintRxPacketError(PROTOCOL_VERSION, dxl_error);

    // Close port
    ClosePort(port_num);

    return 0;
}
