#ifndef _ANIMATION_
#define _ANIMATION_

#include "predef.h"
#include "xmlparser.h"

// speed는 endtime - starttime
struct _MOTION
{
	float		starttime;
	float		endtime;
	uint16_t	angle;
};

//////////////////////////////////////////////////////////////////////////

class Motion 
{
public :
	Motion();
	~Motion();

	bool	init(XMLNode node);
	void	uninit();

	void	update();
	bool	play();

	std::string	getname();
	float	gettotaltime() { return totaltime; }

	void	setstate( int _st ) { state = _st; }
	int		getstate() { return state; }

private:
	bool	_play(int id, int angle, float speed);

	int				state;

	clock_t			starttime;
	float			updatetime;

	float			totaltime;
	std::string		name;

	std::map< int, std::deque<_MOTION> > currentmotion;
	std::map< int, std::deque<_MOTION> > motionlist;
};

//////////////////////////////////////////////////////////////////////////

class Animation
{
public:
	static Animation *getInstance()
	{
		if (instance == NULL)
			instance = new Animation();
		return instance;
	};

	bool	init();
	void	uninit();
	void	update();

	void	load(std::string filename);
	void	play(std::string name);
	bool	isplaying();

private:

	Animation();
	~Animation();

	Motion	*	currentmotion;
	std::map<std::string, Motion*>		animationlist;		// key : motion name , value :   motion

	static Animation *	instance;
};


#endif