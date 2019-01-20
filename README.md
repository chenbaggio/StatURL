# StatURL
get the top 100 high-repeatability URLs in approximately 100G URL files

## Algorithm Description
1. hash urls into different files (one bucket one file), 1G memory handle 100G file, bucket number can set 128 or larger, here take 256
2. select top 100 from every bucket file (count, sort and merge with Previous results)
3. meger all top 100 files and select the global top 100

## Prerequisite
1. not considering charset,  assume the URL file is ASCII
2. not considering sampling to decide which method to apply hash or just sort ,we assume usrl is Low repeatability

## components
1. URLGen.py - generate url file   
  URLGen.py DataFile   
  DataFile - the file to contain URLs   

2. URLStat - stat url and get top n high-repeatability URLs(default top 100)   
URLStat URLFile TmpPath [topN]   
  URLFile - the file that contains the URLs   
  TmpPath - the path that store intermediate results   
  topN - not implemnet   

## compile 
1. git clone https://github.com/chenbaggio/StatURL.git 
2. cd src && ./make.sh
3. run URLStat 

## testing  OS environment

linux redhat 7.2 

