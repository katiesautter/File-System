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

//adding and saving block to cache
int Cache::saveToCache(char name[8], int blockNum, char buf[1024])
{
	for (CacheNode * search = head; search != 0; search = search->next)
	{
		cout << search->toString() << endl;
	}

	//if the cache size is larger than 5 cannot add a new block
	if (currentCacheSize >= 5)
	{
		deleteBlock();
	}
	CacheNode * newCacheNode = new CacheNode(name, blockNum, buf);
	newCacheNode->next = head;
	head = newCacheNode;
	currentCacheSize++;

	
	
}

//getting data from block
char* Cache::getBlock(int blockNum)
{
	for (CacheNode * search = head; search != 0; search = search->next)
	{
		//if the correct blockNumber is found return the data from that block 
		if (search->blockNumber == blockNum)
		{
			return search->blockData;
		}

	}
	cout << "Block number not in cache" << endl;
	return NULL;
	
}
//delete block from cache
void Cache::deleteBlock()
{
	CacheNode * search;
	CacheNode * prev = NULL;

	//deleting last block (first added) from cache
	for (search = head; search != 0; search = search->next)
	{
		if (search->next == NULL)
		{
			delete(search);
			if (prev == NULL)
			{
				head = NULL;
			}
			else
			{
				prev->next = NULL;
			}
		}
		prev = search;
	}
	currentCacheSize--;
}




Cache::~Cache()
{
}
