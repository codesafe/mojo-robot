﻿#ifndef _WHEEL_
#define _WHEEL_

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

#include "part.h"

class Wheel : public Part
{
public :
	Wheel();
	~Wheel();

	bool	init(XMLNode node);
	void	uninit();
	bool	reset();

private:
	bool		torque;
};


#endif
