﻿#ifndef _PART_
#define _PART_

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

#include "predef.h"
#include "xmlparser.h"

class Part
{
public :
	Part();
	virtual ~Part();

	virtual bool	init(XMLNode node);
	virtual void	uninit();
	virtual bool	reset();
	
	int				getid() { return id; }
	std::string		getname() { return name; }

	virtual bool	addsendcommand(uint16_t command, uint16_t param);
	virtual bool	sendcommand(uint16_t command , uint16_t param);
	virtual bool	recvcommand(uint16_t command, uint16_t &param);

	int gettype() { return type; }

protected :
	bool	enable;		// can use ?
	int		id;
	int		type;		// part 종류
	std::string	name;
};

//////////////////////////////////////////////////////////////////////////

class PartController
{
public :

	static PartController *getInstance()
	{
		if (instance == NULL)
			instance = new PartController();
		return instance;
	};

	void	init(XMLNode node);
	void	uninit();

	bool	addsendqueuecommand(uint8_t id, uint16_t command, uint16_t param);
	bool	sendsendqueuecommand();
	bool	sendcommand(int id, uint16_t command, uint16_t param);
	bool	recvcommand(int id, uint16_t command, uint16_t &param);

	void	displaypic(std::string pic);

	Part*	getpart(int type, int id);
	int		getid(std::string name);

private:
	PartController();
	~PartController();

	std::map<int, Part*> partlist;
	static PartController * instance;
};


#endif
