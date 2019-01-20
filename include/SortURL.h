#ifndef SORT_URL_H
#define SORT_URL_H

#include <map>
#include <list>
#include <string>

/*assume one URL 1024B,
256M memory can record the count roughly*/
#define MAX_BATCH_HANDLE_URL_CNT 300000
#define DEFAULT_TOP 100

typedef struct {
	std::string strURL;
	unsigned int nRepeated;
}URL_REPEAT_STAT;

typedef std::list<URL_REPEAT_STAT> LIST_STAT;
typedef std::list<URL_REPEAT_STAT>::iterator LIST_STAT_ITR;

int GetTopNRepeatedURL(char* FileBase,
					int TotalBucket,
					int Top,
					LIST_STAT& lstURLRepeatStat);

int GetTopNRepeatedURLFromHashedFile(char* szFileName,
									int HashBucket,
									LIST_STAT& lstURLRepeatStat,
									int Top /*get top count, for example top 100*/);

void MergeAndGetTopNRepeatedURL(LIST_STAT& lstURLRepeatStatDst, /*merge into*/
								LIST_STAT& lstURLRepeatStatSrc,
								int Top /*get top count, for example top 100*/);


#endif
