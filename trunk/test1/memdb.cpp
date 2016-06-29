#include "memdb.h"

MemDB *	MemDB::instance = NULL;

MemDB::MemDB()
{
}

MemDB::~MemDB()
{
}

int		MemDB::getIntValue(std::string key, int d)
{
	std::string ret = getvalue(key);

	if( ret.empty() )
		return d;

	return atoi(ret.c_str());
}

float	MemDB::getFloatValue(std::string key, float d)
{
	std::string ret = getvalue(key);

	if( ret.empty() )
		return d;

	return (float)atof(ret.c_str());
}

std::string		MemDB::getValue(std::string key)
{
	return getvalue(key);
}

void	MemDB::setValue(std::string key, int value)
{
	char valuestr[256];
	snprintf (valuestr, sizeof (valuestr), "%d", value);

	setvalue(key, valuestr);
}

void	MemDB::setValue(std::string key, float value)
{
	char valuestr[256];
	snprintf (valuestr, sizeof (valuestr), "%f", value);

	setvalue(key, valuestr);
}

void	MemDB::setValue(std::string key, std::string value)
{
	setvalue(key, value);
}

std::string MemDB::getvalue(std::string key)
{
	std::map<std::string, std::string>::iterator it = keyvalue.find(key);
	if( it != keyvalue.end() )
	{
		return it->second;
	}

	return std::string("");
}

void MemDB::setvalue(std::string key, std::string value)
{
	// ������ ���� , ������ ����
	std::map<std::string, std::string>::iterator it = keyvalue.find(key);
	if( it == keyvalue.end() )
	{
		// ��� �߰�
		keyvalue.insert(std::make_pair(key, value));
		return;
	}

	it->second = value;
}