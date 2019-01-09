# StatURL
get the top 100 high-repeatability URLs in approximately 100G URL files

## Algorithm Description
1. hash urls into different files (one bucket one file), 1G memory handle 100G file, bucket number can set 128
2. select top 100 from every bucket file (sort , count and save into files)
3. meger all top 100 files and select the global top 100

## Prerequisite
1. not considering charset
2. not considering sampling to decide which method to apply hash or just sort ,we assume usrl is Low repeatability

## components
1. data - generate url file
GenData -s size(G)

2. UrlStat - stat url and get top n high-repeatability URLs(default top 100)
UrlStat -n top_n

#compile 
1. git clone 
