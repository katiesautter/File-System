#pragma once
class CacheNode
{
public:
	CacheNode(char name[8], int blockNum, char buf[1024]);
	~CacheNode();
	char * toString();
	CacheNode * next;
	int blockNumber;
	char* blockData;
	char * fileName;
	
	
};

