#include "stdafx.h"
#include "CacheNode.h"


CacheNode::CacheNode(int blockNum, char buf[1024])
{
	next = NULL;
	blockNumber = blockNum;
	blockData = buf;

}
char * CacheNode::toString()
{
	char str[1000];
	sprintf_s(str, sizeof(str), "CacheNode: NumBlock= %d", blockNumber);
	return str;

}

CacheNode::~CacheNode()
{
}
