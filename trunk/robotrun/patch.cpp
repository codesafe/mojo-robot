#include "patch.h"
#include "xmlparser.h"
#include "utils.h"

#include <curl/curl.h>

#ifdef WIN32
// curl library
#pragma comment(lib, "libcurl_imp.lib")
#endif

//#define WRITETOFILE
#define MAXDOWNLOAD_BUFFER		1024 * 1024 * 10	// 10Kb

static size_t	downloadedsize;
static char 	downloadbuffer[MAXDOWNLOAD_BUFFER];

Patch::Patch()
{
}

Patch::~Patch()
{
}

void	Patch::dopatch()
{
	currentfileinfo.clear();
	dirtycurrentfileinfo = false;

	// load current patch.xml and collect version info
	bool ret = Utils::checkfileexist(PATCHFILENAME);
	if (ret)
	{
		XMLNode root = XMLNode::openFileHelper(PATCHFILENAME, "");
		for (int i = 0; i < root.nChildNode(); i++)
		{
			XMLNode aninode = root.getChildNode(i);
			const char *version = aninode.getAttribute("version");
			const char *name = aninode.getAttribute("name");
			const char *path = aninode.getAttribute("path");
			const char *crc = aninode.getAttribute("crc");

			DownloadItem item;
			item.version = atoi(version);
			if(crc) item.crc = strtoul(crc, NULL, 16);
			item.name = name;
			item.path = path;
			item.result = false;
			currentfileinfo.push_back(item);
		}
	}

	downloadpatchinfo();
	// 실제 패치 진행
	patchprocess();
}



size_t Patch::writetobuffer(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t wtsize = size * nmemb;
	memcpy(downloadbuffer + downloadedsize, ptr, size * wtsize);
	downloadedsize += wtsize;
	return wtsize;
}

size_t	Patch::writetofile(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

// download patchinfo from patch server
bool	Patch::downloadpatchinfo()
{
	downloadedsize = 0;
	memset(downloadbuffer, 0, MAXDOWNLOAD_BUFFER);

	CURL *curl;
	CURLcode res;
	char *url = PATCHSERVER_ADD;
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Patch::writetobuffer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK)
		{
			Logger::getInstance()->log(LOG_ERR, "Patch download faild.. %d\n", res);
			return false;
		}

		collectnewpatch();
	}

	return true;
}

// collect patch info
void Patch::collectnewpatch()
{
	XMLNode root = XMLNode::parseString(downloadbuffer, "");
	for (int i = 0; i < root.nChildNode(); i++)
	{
		XMLNode node = root.getChildNode(i);
		const char *version = node.getAttribute("version");
		const char *name = node.getAttribute("name");
		const char *path = node.getAttribute("path");
		const char *crc = node.getAttribute("crc");

		DownloadItem item;
		item.version = atoi(version);
		item.crc = strtoul(crc, NULL, 16);
		item.name = name;
		item.path = path;
		item.result = false;
		newpatchfileinfo.push_back(item);
	}
}

// 패치 진행
void	Patch::patchprocess()
{
	// 없는 파일이거나 버전이 낮으면 패치 할것!
	for (size_t i = 0; i < newpatchfileinfo.size(); i++)
	{
		int oldversion = getoldfileversion(newpatchfileinfo[i].name);
		if (oldversion < 0 || newpatchfileinfo[i].version > oldversion)
		{
			willpatchlist.push_back(newpatchfileinfo[i]);
		}
	}

	// donwload Patch files
	for (size_t i = 0; i < willpatchlist.size(); i++)
	{
		bool ret = downloadtofile(willpatchlist[i].path, willpatchlist[i].name);

		// CRC32 검사
		if (ret == true)
		{
			unsigned long crc = Utils::calcCRC32(willpatchlist[i].name);
			if (crc != willpatchlist[i].crc)
				ret = false;
		}


		willpatchlist[i].result = ret;

		if (ret == true)
		{
			if (getoldfileversion(willpatchlist[i].name) == -1)
			{
				// 이전에 없던거라 추가
				currentfileinfo.push_back(willpatchlist[i]);
				dirtycurrentfileinfo = true;
			}
			else
			{
				// 있어서 업데이트
				for (size_t j = 0; j < currentfileinfo.size(); j++)
				{
					if (currentfileinfo[j].name == willpatchlist[i].name)
					{
						currentfileinfo[j].crc = willpatchlist[i].crc;
						currentfileinfo[j].name = willpatchlist[i].name;
						currentfileinfo[j].path = willpatchlist[i].path;
						currentfileinfo[j].version = willpatchlist[i].version;
						dirtycurrentfileinfo = true;
						break;
					}
				}
			}
		}
	}

	// result가 정상인것들은 patch.xml에 업데이트
	updateoldpatch();
}

int		Patch::getoldfileversion(std::string newfile)
{
	for (size_t i = 0; i < currentfileinfo.size(); i++)
	{
		if (currentfileinfo[i].name == newfile)
		{
			return currentfileinfo[i].version;
		}
	}
	
	return -1;
}

// 실제 다운로드
bool	Patch::downloadtofile(std::string url, std::string outfilename)
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		Logger::getInstance()->log(LOG_INFO, "Patch : Download progress.\n%s --> %s\n", url.c_str(), outfilename.c_str());
		FILE *fp = fopen(outfilename.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Patch::writetofile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
		if (res != CURLE_OK)
		{
			Logger::getInstance()->log(LOG_ERR, "Patch download faild.. %d\n", res);
			return false;
		}
	}
	return true;
}

void	Patch::updateoldpatch()
{
	if (dirtycurrentfileinfo)
	{
		XMLNode root = XMLNode::createXMLTopNode("patch");
		for (size_t i = 0; i < currentfileinfo.size(); i++)
		{
			XMLNode node = root.addChild("file");
			char buffer[64];
			sprintf(buffer, "%d", currentfileinfo[i].version);
			node.addAttribute("version", buffer);
			node.addAttribute("name", currentfileinfo[i].name.c_str());
			node.addAttribute("path", currentfileinfo[i].path.c_str());
			sprintf(buffer, "%x", currentfileinfo[i].crc);
			node.addAttribute("crc", buffer);
		}

		root.writeToFile("patch.xml");
	}

	dirtycurrentfileinfo = false;
}