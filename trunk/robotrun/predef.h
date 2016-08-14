#ifndef _PREDEF_

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ctime>
#include <deque>
#include <string.h> 

// Uses Dynamixel SDK library
#include "dynamixel_sdk.h"
#include "memdb.h"
#include "logger.h"


//using namespace std;
//using namespace dynamixel;

//////////////////////////////////////////////////////////////////////////


#define	TESTBUILD


#define ESC_ASCII_VALUE			0x1b
#define PROTOCOL_VERSION		1.0
//#define PROTOCOL_VERSION		2.0

#define DEVICE_DISABLE			0
#define DEVICE_ENABLE			1


// Control Address
#define TORQUEMODE				24	// 토크 모드
#define CW_LIMIT_ANGLE			6
#define CCW_LIMIT_ANGLE			8
#define DEST_POSITION			30	// 0 ~ 4096
#define MOVE_SPEED				32	// 0 ~ 1024
#define CURRENT_POSITION		36	// 현재 위치 read

#define D_PARAM					26	// PID - D
#define I_PARAM					27	// PID - I
#define P_PARAM					28	// PID - P


// 0~1023 범위의 값을 사용하면 CCW방향으로 회전하며 0으로 설정하면 정지합니다.
// 1024~2047 범위의 값을 사용하면 CW방향으로 회전하며 1024으로 설정하면 정지합니다.
#define WHEEL_SPEED				32	

// part type list
#define PART_TYPE_NONE			0
#define PART_TYPE_DISPLAY		0x10		// e-Ink Display
#define PART_TYPE_JOINT			0x20		// 팔 / 목 / 얼굴 관절
#define PART_TYPE_WHEEL			0x30		// 구동(바퀴) 부


// 동작관련 수치 변형 Macro
#define ROT_VALUE				(11.377)		// 4096 / 360 (360 to 4096)
#define DEGREE2DXL(A)			(uint16_t)(A*ROT_VALUE)	// degree to dxl value
#define DXL2DEGREE(A)			(uint16_t)(A/ROT_VALUE)	// dxl value to degree

// 회전 Speed 설정에 필요한 DXL 값 얻기
// N초에 R만큼 회전하려 할때의 RPM / RPM 설정값
// R / (6 * n) = RPM
// R / (0.114 * 6 * n) = RPM 설정값
#define ROTSPEED				0.114
// R은 회전량 절대값 <-- ( abs(현재 각도 - 목표 각도) ) 
#define SPEEDVALUE(N, R)		(uint16_t)(R / (ROTSPEED * 6 * N ))


// animation state		
#define STATE_IDLE			0		// 최초 정지상태
#define STATE_RUNNING		1		// 현재 에니메이션 중
#define STATE_STOPPED		2		// 에니메이션 완료 후 정지


#define SERVER_ADD			"127.0.0.1"
#define SERVER_PORT			8888
#define SOCKET_BUFFER		4096


// network packet

// response packet
#define RESPONSE_OK		0x05
#define RESPONSE_FAIL	0x06

// command packet
#define DEVICERESET		0x10
#define	PLAYMOTION		0x20
#define WHEEL_FORWARD	0x30
#define WHEEL_BACKWARD	0x31
#define DISPLAY_PIC		0x40


// Command type
#define COMMAND_DEVICE		0x10
#define COMMAND_ANIMATION	0x20
#define COMMAND_WHEEL		0x30
#define COMMAND_DISPLAY		0x40


#endif
