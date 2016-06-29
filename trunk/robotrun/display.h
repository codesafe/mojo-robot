#ifndef _DISPLAY_
#define _DISPLAY_

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

#include "part.h"

class Display : public Part
{
public :
	Display();
	~Display();

	bool	init(XMLNode node);
	void	uninit();
	bool	reset();

	bool	sendcommand(uint16_t command, uint16_t param);
	bool	recvcommand(uint16_t command, uint16_t &param);

private:

};


#endif
