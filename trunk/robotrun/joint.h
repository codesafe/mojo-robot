#ifndef _JOINT_
#define _JOINT_

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

#include "part.h"

class Joint : public Part
{
public :
	Joint();
	~Joint();

	bool	init(XMLNode node);
	void	uninit();
	bool	reset();

	bool	addsendcommand(uint16_t command, uint16_t param);
	bool	sendcommand(uint16_t command , uint16_t param);
	bool	recvcommand(uint16_t command, uint16_t &param);

private:
	bool		torque;
	uint16_t	initpos;
	uint16_t	cwlimit;
	uint16_t	ccwlimit;
};


#endif
