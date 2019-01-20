#include "HashURL.h"
#include <new>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <assert.h>

static int HashString(const char* String, int Size, int Mod)
{
	unsigned long h = 0;

	for (int i = 0; i < Size; i++)
	{
		h = (h << 4) + String[i];

		unsigned long g = h & 0xF0000000L;

		if(g)
		{
			h ^= g >> 24;
		}

		h &= ~g;
	}

	return h % Mod;
}

int HandleURLFileByHash(const char* szFileName, const char* szTmpPath)
{
	assert(szFileName != NULL);

	unsigned long BufSize = 1024 * 1024 * 1024;

	char *pBuf = new(std::nothrow) char[BufSize];

	if (pBuf == NULL)
	{
		return -1;
	}

	FILE_HASH_MANAGER * pFHM = new FILE_HASH_MANAGER[TOTAL_BUCKETS_CNT];

	if (pFHM == NULL)
	{
		delete[] pBuf;
		return -1;
	}

	int i = 0;
	char szHashFile[PATH_MAX] = {0};

	/*setup for work*/
	for (i = 0; i < TOTAL_BUCKETS_CNT; i++)
	{
		snprintf(szHashFile, sizeof(szHashFile) - 1, "%s/%d", szTmpPath, i);
		pFHM[i].Setup(pBuf + (BUCKET_BUFFER_SIZE * i),
				szHashFile,
				i);
	}

	std::fstream URLFile;
	char URLBuf[URL_HANDLER_BUFFER_SIZE] = {0};
	URLFile.open(szFileName, std::ios::in|std::ios::out|std::ios::binary);

	if (!URLFile.is_open())
	{
		delete[] pBuf;
		delete[] pFHM;
		return -1;
	}

	/*hash line by line and save to bucket file*/
	//while (!URLFile.eof())
	while (URLFile.getline (URLBuf, sizeof(URLBuf)))
	{
		//URLFile.getline (URLBuf, sizeof(URLBuf));
		int Len = strlen(URLBuf);
		int HashVal = HashString(URLBuf, Len, TOTAL_BUCKETS_CNT);
		URLBuf[Len] = '\n';
		pFHM[HashVal].AppendBuf(URLBuf, (Len + 1));
	}

	delete[] pFHM;
	delete[] pBuf;
	URLFile.close();

	return 0;
}

void ClearAllHashFile(const char* szBase, int TotalBucket)
{
	char szFileName[PATH_MAX] = {0};

	for (int i = 0; i < TotalBucket; i++)
	{
		snprintf(szFileName, sizeof(szFileName), "%s/%d",
				szBase, i);
		unlink(szFileName);
	}

	return;
}
