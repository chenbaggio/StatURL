#include "SortURL.h"
#include "HashURL.h"
#include <unistd.h>
#include <map>
#include <stdio.h>
#include <limits.h>
#include <fstream>

bool ListSort(URL_REPEAT_STAT& Comp1,
			URL_REPEAT_STAT& Comp2)
{
	return Comp2.nRepeated < Comp1.nRepeated;
}

int GetTopNRepeatedURL(char* FileBase,
					int TotalBucket,
					int Top,
					LIST_STAT& lstURLRepeatStat)
{
	int i = 0;

	char szFileName[PATH_MAX] = {0};

	while (i < TotalBucket)
	{
		snprintf(szFileName, sizeof(szFileName) - 1, "%s/%d",
				FileBase, i);

		printf("start handle file: %s\n", szFileName);

		 int Ret = GetTopNRepeatedURLFromHashedFile(
				 	 	 	 szFileName, i, lstURLRepeatStat, Top);
		if (Ret < 0)
		{
			return -1;
		}
		else if (Ret == 0)
		{
			i += 1;
		}
		else
		{
			//nothing to do
		}
	}

	return 0;
}

int GetTopNRepeatedURLFromHashedFile(char* szFileName,
									int HashBucket,
									LIST_STAT& lstURLRepeatStat,
									int Top /*get top count, for example top 100*/)
{
	/*check arguments*/
	if (szFileName == NULL)
	{
		return -1;
	}

	int Ret = 0;

	std::map<std::string, int> mapStat;
	std::pair<std::map<std::string,int>::iterator, bool>  mapInsert;
	std::map<std::string,int>::iterator mapItr;

	std::fstream HashFile;
	char URLBuf[URL_HANDLER_BUFFER_SIZE] = {0};

	HashFile.open(szFileName, std::ios::in|std::ios::out|std::ios::binary);

	if (!HashFile.is_open())
	{
		return -1;
	}

	FILE_HASH_MANAGER* pFHM = NULL;

	char szFileNameTmp[PATH_MAX] = {0};

	snprintf(szFileNameTmp, sizeof(szFileNameTmp) - 1,
			"%s_tmp", szFileName);

	/*hash line by line and save to bucket file*/
	//while (!HashFile.eof())
	while (HashFile.getline (URLBuf, sizeof(URLBuf)))
	{
		//HashFile.getline (URLBuf, sizeof(URLBuf));
		std::string strTmp = URLBuf;
		if (mapStat.size() < MAX_BATCH_HANDLE_URL_CNT)
		{
			mapInsert = mapStat.insert(std::make_pair(strTmp, 1));

			if (!mapInsert.second)
			{
				mapInsert.first->second += 1;
			}
		}
		else
		{
			/*handle the URL already in map
			 * those not in map would be save to temporary file
			 * and handle them lately*/
			mapItr = mapStat.find(strTmp);

			if (mapItr != mapStat.end())
			{
				mapItr->second += 1;
			}
			else
			{
				if (pFHM == NULL)
				{
					pFHM = new(std::nothrow) FILE_HASH_MANAGER;

					if (pFHM == NULL)
					{
						HashFile.close();
						return -1;
					}

					if (pFHM->Setup(NULL, szFileNameTmp, HashBucket) != 0)
					{
						HashFile.close();
						delete pFHM;
						return -1;
					}
				}

				pFHM->AppendBuf(strTmp.c_str(), strTmp.length());
			}
		}

	}

	HashFile.close();

	std::multimap<int, std::string> MultiMapStat;

	for(mapItr = mapStat.begin(); mapItr != mapStat.end(); mapItr++)
	{
		MultiMapStat.insert(std::make_pair((*mapItr).second * (-1), (*mapItr).first));
	}

	LIST_STAT lstURLRepeatStatTmp;
	std::multimap<int, std::string>::iterator multimapItr = MultiMapStat.begin();

	for (int i = 0; (i < Top) && (multimapItr != MultiMapStat.end()); i++, multimapItr++)
	{
		URL_REPEAT_STAT StatTmp;
		StatTmp.strURL = multimapItr->second;
		StatTmp.nRepeated = multimapItr->first * (-1);
		//printf("bucket file %d top %d is %s   %d\n", HashBucket, i, StatTmp.strURL.c_str(), StatTmp.nRepeated);
		lstURLRepeatStatTmp.push_back(StatTmp);
	}

	MergeAndGetTopNRepeatedURL(lstURLRepeatStat, lstURLRepeatStatTmp, Top);

	/*delete files*/
	unlink(szFileName);

	/*remain URL restore to bucket file to handle again
	 *instead of K-Way Merge*/
	if (pFHM != NULL)
	{
		pFHM->finish();
		rename(szFileNameTmp, szFileName);
		Ret = 1;
	}

	return Ret;
}

void MergeAndGetTopNRepeatedURL(LIST_STAT& lstURLRepeatStatDst, /*merge into*/
								LIST_STAT& lstURLRepeatStatSrc,
								int Top /*get top count, for example top 100*/)
{

	lstURLRepeatStatDst.merge(lstURLRepeatStatSrc, &ListSort);

	/*get top N*/
	LIST_STAT_ITR ItrDst = lstURLRepeatStatDst.begin();

	for (int i = 0; (i < Top) && (ItrDst != lstURLRepeatStatDst.end()); i++)
	{
		//printf("after merge list url is %s repeated count is %d\n",
		//		(*ItrDst).strURL.c_str(), (*ItrDst).nRepeated);
		ItrDst++;

	}

	/*erase those behind top N*/
	lstURLRepeatStatDst.erase(ItrDst, lstURLRepeatStatDst.end());

	return;
}



