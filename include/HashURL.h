#ifndef HASH_URL_H
#define HASH_URL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <new>

#define TOTAL_BUCKETS_CNT 256
#define BUCKET_BUFFER_SIZE (4 * 1024 * 1024)
#define URL_HANDLER_BUFFER_SIZE (1024)


#define IS_BUF_FULL(cur, len) ((BUCKET_BUFFER_SIZE - cur) <= len ? true : false)

typedef unsigned long long ull;

typedef struct _stFileHashManager{
	/*construct*/
	_stFileHashManager() {_CurBufPos = 0; _Buf = NULL; _Fd = -1; _Del = false;}

	/*deconstruct*/
	~_stFileHashManager()
	{
		finish();
	}

	/*ifcs*/
	int Setup(char* p, const char* FileName, int Bucket)
	{
		if (p == NULL)
		{
			_Buf = new(std::nothrow) char[BUCKET_BUFFER_SIZE];

			if (_Buf == NULL)
			{
				return -1;
			}
		}
		else
		{
			_Buf = p;
		}

		if (FileName != NULL)
		{
			snprintf(_FileName, sizeof(_FileName), "%s", FileName);
		}
		else
		{
			char* pCur = getenv("PWD");

			if (pCur != NULL)
			{
				snprintf(_FileName, sizeof(_FileName) - 1, "%s/%d", pCur, Bucket);
			}
		}

		_Bucket = Bucket;

		return 0;
	}

	int AppendBuf(const char* pContent, int Size)
	{
		if ((_Buf != NULL) && (pContent != NULL) && (Size > 0))
		{
			/*check whether buffer is full*/
			if (IS_BUF_FULL(_CurBufPos, Size))
			{
				if (FLushBuf() != 0)
				{
					return -1;
				}
			}

			memcpy(_Buf + _CurBufPos, pContent, Size);
			_CurBufPos += Size;
		}

		return 0;
	}

	int FLushBuf()
	{
		if (_Fd == -1)
		{
			_Fd = open(_FileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

			if (_Fd == -1)
			{
				return -1;
			}
		}

		if (_CurBufPos > 0)
		{
			if (write(_Fd, _Buf, _CurBufPos) < 0)
			{
				return -1;
			}

			_CurBufPos = 0;
		}

		return 0;

	}

	int finish()
	{
		FLushBuf();

		if (_Fd != -1)
		{
			close(_Fd);
		}

		if (_Del)
		{
			delete[] _Buf;
		}

		return 0;
	}

	/*members*/
	int _Bucket;
	int _CurBufPos;
	int _Fd;
	char* _Buf;
	bool _Del;
	char _FileName[PATH_MAX];

}FILE_HASH_MANAGER;

int HandleURLFileByHash(const char* szFileName, const char* szTmpPath);
void ClearAllHashFile(const char* szTmp, int TotalBucket);

#endif
