
#include <fstream>
#include <stdio.h>
#include <string>
#include <limits.h>
#include <errno.h>
#include "SortURL.h"
#include "HashURL.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("please enter the URL file name\n");
		return -1;
	}

	char szFileName[PATH_MAX] = {0};
	char szFileBase[PATH_MAX] = {0};

	snprintf(szFileName, sizeof(szFileName) - 1, "%s", argv[1]);

	if (argc > 2)
	{
		snprintf(szFileBase, sizeof(szFileBase) - 1, "%s", argv[2]);
	}
	else
	{
		char* pCur = getenv("PWD");
		snprintf(szFileBase, sizeof(szFileBase) - 1, "%s", pCur);
	}

	printf("start hash URL to hash files\n");

	if (HandleURLFileByHash(szFileName, szFileBase) != 0)
	{
		printf("hash URL to hash files fail, process exit\n");
		return -1;
	}

	printf("finish hash URL to hash files\n");

	LIST_STAT lstResult;

	if (GetTopNRepeatedURL(szFileBase,
							TOTAL_BUCKETS_CNT,
							DEFAULT_TOP,
							lstResult) != 0)
	{
		ClearAllHashFile(szFileBase, TOTAL_BUCKETS_CNT);
		printf("get top %d repeated URL fail\n", DEFAULT_TOP);
		return -1;
	}

	LIST_STAT_ITR lstItr;

	printf("the top %d is:\n", DEFAULT_TOP);

	for (lstItr = lstResult.begin(); lstItr != lstResult.end(); lstItr++)
	{
		printf("\t%s %d\n", (*lstItr).strURL.c_str(), (*lstItr).nRepeated);
	}
}
