#ifndef _ANIMATION_
#define _ANIMATION_

#include "predef.h"
#include "xmlparser.h"

// speed는 endtime - starttime
struct _MOTION
{
	int			type;
	float		starttime;
	float		endtime;
	uint16_t	angle;
	std::string	note;		// for display
};

//////////////////////////////////////////////////////////////////////////

class Motion 
{
public :
	Motion();
	~Motion();

	bool	init(XMLNode node);
	void	uninit();

	bool	update();
	bool	play();
	void	stop();

	std::string	getname();
	float	gettotaltime() { return totaltime; }

	void	setstate( int _st ) { state = _st; }
	int		getstate() { return state; }

private:
	bool	_play(int id, _MOTION &motion);

	int				state;

	double			starttime;
	float			updatetime;

	float			totaltime;
	std::string		name;

	std::map< int, std::deque<_MOTION> > currentmotion;
	std::map< int, std::deque<_MOTION> > motionlist;
};

//////////////////////////////////////////////////////////////////////////

struct ANIFILE
{
	int	version;
	std::string		filename;
};


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
	void	stop();
	bool	isplaying();


private:

	Animation();
	~Animation();

	Motion	*	currentmotion;
	std::map<std::string, Motion*>		animationlist;		// key : motion name , value :   motion
	std::vector<ANIFILE>				animationfilelist;

	static Animation *	instance;
};


#endif