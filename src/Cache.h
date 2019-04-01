#pragma once
#include "CacheNode.h"
class Cache
{
public:
	Cache();
	void saveToCache(int blockNum, char buf[1024]);
	char* getBlock(int blockNum);
	~Cache();

private:
	int maxCacheSize;
	int currentCacheSize;
	CacheNode * head;
	
};



