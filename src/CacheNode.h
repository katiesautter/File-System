#pragma once
class CacheNode
{
public:
	CacheNode(int blockNum, char buf[1024]);
	~CacheNode();

private:
	int blockNumber;
	char* blockData;
	CacheNode * next;

};

