#ifndef _PATCH_
#define _PATCH_

#include "predef.h"

struct DownloadItem
{
	int				version;
	unsigned long	crc;
	std::string		path;
	std::string		name;
	bool			result;

	DownloadItem()
	{
		version = 1;
		crc = 0;
		result = false;
	}
};

class Patch
{
public :
	Patch();
	~Patch();

	void	dopatch();

private :

	bool	downloadpatchinfo();
	void	patchprocess();
	void	collectnewpatch();
	int		getoldfileversion(std::string newfile);
	bool	downloadtofile(std::string url, std::string outfilename);
	void	updateoldpatch();

	static size_t writetofile(void *ptr, size_t size, size_t nmemb, FILE *stream);
	static size_t writetobuffer(void *ptr, size_t size, size_t nmemb, FILE *stream);

	bool dirtycurrentfileinfo;
	std::vector<DownloadItem> currentfileinfo;
	std::vector<DownloadItem> newpatchfileinfo;
	std::vector<DownloadItem> willpatchlist;
};



#endif