#include "stdafx.h"
#include "Cache.h"
#include <iostream>

using namespace std;


Cache::Cache()
{
	cout << "instantiated cache" << endl;
	maxCacheSize = 5;
	currentCacheSize = 0;
	head = NULL;

}

void Cache::saveToCache(int blockNum, char buf[1024])
{
	head = new CacheNode(blockNum, buf);
	//cout << head->toString() << endl;
}

char* Cache::getBlock(int blockNum)
{
	return 0;
}




Cache::~Cache()
{
}
