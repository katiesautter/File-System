#pragma once
#include "CacheNode.h"
class Cache
{
public:
	Cache();
	int saveToCache(char name[8], int blockNum, char buf[1024]);
	char* getBlock(int blockNum);
	~Cache();
	int maxCacheSize;
	int currentCacheSize;
	CacheNode * head;
	
	
};



